#include "stdafx.h"

#include <filesystem>
#include <algorithm>
#include <GLFW/glfw3.h>
#include <optick/optick.h>

#include "gcore/resource_library.h"

#include "gserializer/serializer.h"
#include "gserializer/serializers/json_serializer.h"

#include "gcore/serializers/dependency_gatherer_serializer.h"



namespace gcore
{
    resource_library::resource_library(GLFWwindow* rendering_context)
    {
        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
        m_loading_context = glfwCreateWindow(1, 1, "", nullptr, rendering_context);
        m_jobs.submit([=] {
            glfwMakeContextCurrent(m_loading_context);
        });
    }

    resource_library::~resource_library()
    {
        {
            std::unique_lock lock(m_lock);
            for (auto& proxy : m_proxy_map)
            {
                if (auto shared_proxy = proxy.second.lock())
                {
                    shared_proxy->set_target(nullptr);
                }
            }

            for (auto& res : m_resource_map)
            {
                if (res.second->get_state() == resource::loading_state::loaded)
                    res.second->unload();
            }

            for (auto& res : m_res_to_unload)
            {
                if (res->get_state() == resource::loading_state::loaded)
                    res->unload();
            }
        }

        m_jobs.stop_and_join();

        {
            std::unique_lock lock(m_lock);
            for (auto& res : m_res_to_load_sync)
            {
                res->load_sync();
                if (res->get_state() == resource::loading_state::loaded)
                    res->unload();
            }
        }


        glfwDestroyWindow(m_loading_context);
    }

    void resource_library::process(gserializer::serializer& serializer)
    {
        serializer.process("uuid_to_resource_file", m_uuid_to_resource_file, "Resource");
    }

    resource_handle<resource> resource_library::try_get_resource(gtl::uuid const& uuid)
    {
        {
            std::shared_lock lock(m_lock);
            if (auto const it = m_proxy_map.find(uuid);
                it != m_proxy_map.end())
            {
                if (auto shared_proxy = it->second.lock())
                {
                    return resource_handle<resource>(std::move(shared_proxy));
                }
            }
        }
        return resource_handle<resource>();
    }

    resource_handle<resource> resource_library::get_resource(gtl::uuid const& uuid)
    {
        if (auto handle = try_get_resource(uuid))
            return handle;

        std::unique_lock lock(m_lock);
        auto const& [it, inserted] = m_proxy_map.insert(std::pair(uuid, std::weak_ptr<resource_proxy>()));
        std::shared_ptr<resource_proxy> proxy;
        if (inserted)
        {
            if (auto const file_it = m_uuid_to_resource_file.find(uuid);
                file_it != m_uuid_to_resource_file.end())
            {
                std::unique_ptr<resource> resource_to_load;
                gserializer::json_read_serializer serializer(file_it->second.string().c_str());
                serializer.process("resource", resource_to_load, resource::factory());
                if (resource_to_load)
                {
                    proxy = std::make_shared<resource_proxy>(resource_to_load.get(), this);
                    load_resource(std::move(resource_to_load));
                    it->second = proxy;
                }
            }
        }
        return resource_handle<resource>(it->second.lock());
    }

    std::filesystem::path resource_library::get_filepath(gtl::uuid const& uuid) const
    {
        if (auto const it = m_uuid_to_resource_file.find(uuid);
            it != m_uuid_to_resource_file.end())
        {
            return it->second;
        }
        return std::string();
    }

    void resource_library::scan_directory(const char* directory)
    {
        namespace fs = std::filesystem;
        for (auto& path : fs::recursive_directory_iterator(directory))
        {
            if (path.is_regular_file() && path.path().extension() == fs::path(".json"))
            {
                gtl::uuid const uuid = gtl::uuid::from_string(path.path().stem().string());
                if (uuid.is_valid())
                {
                    m_uuid_to_resource_file[uuid] = path.path();
                }
            }
        }
    }

    void resource_library::on_file_change(std::wstring const& path)
    {
        auto absolute_path = std::filesystem::absolute(path).make_preferred();
        std::unique_lock lock(m_file_change_lock);
        if (auto it = std::find(m_file_changes.begin(), m_file_changes.end(), path);
            it == m_file_changes.end())
        {
            m_file_changes.push_back(std::move(absolute_path));
        }
    }

    void resource_library::update()
    {
        OPTICK_EVENT();
        {
            std::unique_lock file_lock(m_file_change_lock);
            for (auto const& path : m_file_changes)
            {
                if (path.extension() == std::filesystem::path(".json"))
                {
                    gtl::uuid const uuid = gtl::uuid::from_string(path.stem().string());
                    if (uuid.is_valid())
                    {
                        std::unique_lock lock(m_lock);
                        if (auto const it = m_proxy_map.find(uuid);
                            it != m_proxy_map.end())
                        {
                            reload_resource(uuid);
                        }
                        m_uuid_to_resource_file[uuid] = path;
                    }
                }

                for (auto const& uuid : m_dependency_tracker.get_uuid_depending_on_file(path))
                {
                    std::unique_lock lock(m_lock);
                    if (auto proxy_it = m_proxy_map.find(uuid); 
                        proxy_it != m_proxy_map.end())
                    {
                        reload_resource(uuid);
                    }
                }
            }
            m_file_changes.clear();

            std::vector<std::unique_ptr<resource>> resource_to_unload;
            {
                std::unique_lock lock(m_lock);
                for (auto& res_to_load : m_res_to_load_sync)
                {
                    res_to_load->load_sync();
                    // if there is no more proxy it means no one needs the resource now
                    if (auto const proxy_it = m_proxy_map.find(res_to_load->get_uuid());
                        proxy_it != m_proxy_map.end())
                    {
                        if (std::shared_ptr<resource_proxy> proxy = proxy_it->second.lock()) // proxy may be waiting to be deleted so need to check the lock
                        {
                            auto [it, inserted] = m_resource_map.insert(std::pair(res_to_load->get_uuid(), std::unique_ptr<resource>()));
                            if (!inserted)  //replacing old resource need to unload old one and rebind proxy
                            {
                                proxy->set_target(res_to_load.get());
                                m_res_to_unload.push_back(std::move(it->second));
                                on_resource_reloaded(res_to_load->get_uuid());
                            }
                            it->second = std::move(res_to_load);
                            continue;
                        }
                    }

                    m_res_to_unload.push_back(std::move(res_to_load));
                }
                m_res_to_load_sync.clear();

                resource_to_unload = std::move(m_res_to_unload); // Unloading resource can trigger the unloading of more resource. Move the data to the stack and unlock the lock to avoid deadlock and iterating over a modified vector.
            }

            for (std::unique_ptr<resource>& res : resource_to_unload)
            {
                if (res->get_state() == resource::loading_state::loaded)
                    res->unload();
            }
        }
    }

    void resource_library::run_loading_job()
    {
        assert(m_jobs.is_currently_on_job_thread());
        m_jobs.try_run_job();
    }

    void resource_library::request_unload(resource* res_to_unload)
    {
        std::unique_lock lock(m_lock);
        m_proxy_map.erase(res_to_unload->get_uuid());
        if(auto it = m_resource_map.find(res_to_unload->get_uuid());
            it != m_resource_map.end())
        {
            m_res_to_unload.push_back(std::move(it->second));
            m_resource_map.erase(it);
        }
    }

    void resource_library::load_resource(std::unique_ptr<resource> res_to_load)
    {
        res_to_load->set_library(this);
        m_dependency_tracker.on_resource_loaded(*res_to_load);

        //small workaround to go around copy requirement of std::function and the inability to go from shared_ptr to unique_ptr
        m_jobs.submit([res = std::shared_ptr<resource>(res_to_load.release(), [](resource*) {}), this]() mutable
            {
                assert(res.use_count() == 1);
                std::unique_ptr<resource> real_owner = std::unique_ptr<resource>(res.get());
                real_owner->load_async();
                std::unique_lock lock(m_lock);
                m_res_to_load_sync.push_back(std::move(real_owner));
            }
        );
    }

    void resource_library::reload_resource(gtl::uuid const& uuid)
    {
        if (auto file_path = m_uuid_to_resource_file.find(uuid);
            file_path != m_uuid_to_resource_file.end())
        {
            std::unique_ptr<resource> new_resource;
            gserializer::json_read_serializer serializer(file_path->second.string().c_str());
            serializer.process("resource", new_resource, resource::factory());
            if (new_resource)
            {
                load_resource(std::move(new_resource));
                for (auto const& dependant_uuid : m_dependency_tracker.get_uuid_depending_on_uuid(uuid))
                {
                    auto it = m_proxy_map.find(dependant_uuid);
                    if (it != m_proxy_map.end())
                    {
                        reload_resource(dependant_uuid);
                    }
                }
            }
        }
    }
}
