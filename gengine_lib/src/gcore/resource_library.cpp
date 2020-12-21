#include "stdafx.h"

#include <filesystem>
#include <algorithm>

#include "gcore/resource_library.h"

#include "gserializer/serializer.h"
#include "gserializer/serializers/json_serializer.h"

#include "gcore/serializers/dependency_gatherer_serializer.h"

#include <GLFW/glfw3.h>

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
        glfwDestroyWindow(m_loading_context);
    }

    void resource_library::process(gserializer::serializer& serializer)
    {
        serializer.process("uuid_to_resource_file", m_uuid_to_resource_file, "Resource");
    }

    resource_handle<resource> resource_library::get_resource(gtl::uuid const& uuid)
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

        std::unique_lock lock(m_lock);
        auto const& [it, inserted] = m_resource_map.insert(std::pair(uuid, std::unique_ptr<resource>()));
        std::shared_ptr<resource_proxy> proxy;
        if (inserted)
        {
            if (auto const file_it = m_uuid_to_resource_file.find(uuid);
                file_it != m_uuid_to_resource_file.end())
            {
                gserializer::json_read_serializer serializer(file_it->second.c_str());
                serializer.process("resource", it->second, resource::factory());
                if (it->second)
                {
                    if (it->second->need_async_load())
                    {
                        m_jobs.submit([resource = it->second.get(), this]
                            {
                                resource->load_async();
                                if (resource->get_state() == resource::loading_state::pending_sync)
                                {
                                    std::unique_lock lock(m_lock);
                                    m_res_to_load_sync.push_back(resource);
                                }
                            }
                        );
                    }
                    else
                    {
                        it->second->load_sync();
                    }

                    gcore::dependency_gatherer_serializer dependency;
                    dependency.process("resource", *(it->second));
                    for (auto const& uuid : dependency.m_uuids)
                    {
                        if (uuid != it->second->get_uuid() && uuid.is_valid())
                        {
                            m_uuid_dependant_map[uuid].push_back(it->second->get_uuid());
                        }
                    }
                    for (auto const& file : dependency.m_files)
                    {
                        m_file_dependant_map[std::filesystem::hash_value(std::filesystem::absolute(file).make_preferred())].push_back(it->second->get_uuid());
                    }

                    proxy = std::make_shared<resource_proxy>(it->second.get(), this);
                    m_proxy_map[uuid] = proxy;
                }
            }
        }
        else
        {
            proxy = m_proxy_map[uuid].lock();
        }
        return resource_handle<resource>(std::move(proxy));
    }

    std::string resource_library::get_filepath(gtl::uuid const& uuid) const
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
                    m_uuid_to_resource_file[uuid] = path.path().string();
                }
            }
        }
    }

    void resource_library::on_file_change(std::wstring const& path)
    {
        std::unique_lock lock(m_file_change_lock);
        m_file_changes.push_back(std::filesystem::absolute(path).make_preferred());
    }

    void resource_library::update()
    {
        {
            std::unique_lock lock(m_file_change_lock);
            for (auto const& path : m_file_changes)
            {
                if (path.extension() == std::filesystem::path(".json"))
                {
                    gtl::uuid const uuid = gtl::uuid::from_string(path.stem().string());
                    if (uuid.is_valid())
                    {
                        std::unique_lock lock(m_lock);
                        auto it = m_resource_map.find(uuid);
                        if (it != m_resource_map.end())
                        {
                            gserializer::json_read_serializer serializer(path.string().c_str());
                            serializer.process("resource", it->second, resource::factory());
                            it->second->unload();
                            it->second->load_async();
                            it->second->load_sync();
                        }

                        auto uuid_it = m_uuid_dependant_map.find(uuid);
                        if (uuid_it != m_uuid_dependant_map.end())
                        {
                            for (auto const& uuid : uuid_it->second)
                            {
                                auto it = m_resource_map.find(uuid);
                                if (it != m_resource_map.end())
                                {
                                    it->second->unload();
                                    it->second->load_async();
                                    it->second->load_sync();
                                }
                            }
                        }
                    }
                }
                auto it = m_file_dependant_map.find(std::filesystem::hash_value(path));
                if (it != m_file_dependant_map.end())
                {
                    for (auto const& uuid : it->second)
                    {
                        std::unique_lock lock(m_lock);
                        auto it = m_resource_map.find(uuid);
                        if (it != m_resource_map.end())
                        {
                            it->second->unload();
                            it->second->load_async();
                            it->second->load_sync();
                        }
                    }
                }
            }
            m_file_changes.clear();
        }

        {
            std::unique_lock lock(m_lock);
            for (auto& res_to_load : m_res_to_load_sync)
            {
                res_to_load->load_sync();
            }
            m_res_to_load_sync.clear();
            
            m_res_to_unload.erase(
                std::remove_if(
                    m_res_to_unload.begin(), m_res_to_unload.end(), 
                    [](std::unique_ptr<resource> const& res)
                        {
                            if (res->get_state() == resource::loading_state::loaded)
                            {
                                res->unload();
                                return true;
                            }
                            return res->get_state() == resource::loading_state::failed;
                        })
                , m_res_to_unload.end());
        }
    }

    void resource_library::request_unload(resource* res_to_unload)
    {
        std::unique_lock lock(m_lock);
        m_proxy_map.erase(res_to_unload->get_uuid());
        auto it = m_resource_map.find(res_to_unload->get_uuid());
        if (it != m_resource_map.end())
        {
            m_res_to_unload.push_back(std::move(it->second));
            m_resource_map.erase(it);
        }
    }
}
