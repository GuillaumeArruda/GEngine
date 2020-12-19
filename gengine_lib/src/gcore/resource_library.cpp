#include "stdafx.h"

#include <filesystem>

#include "gcore/resource_library.h"

#include "gserializer/serializer.h"
#include "gserializer/serializers/json_serializer.h"

#include "gcore/serializers/dependency_gatherer_serializer.h"

namespace gcore
{
    void resource_library::process(gserializer::serializer& serializer)
    {
        serializer.process("uuid_to_resource_file", m_uuid_to_resource_file, "Resource");
    }

    resource* resource_library::get_resource(gtl::uuid const& uuid)
    {
        {
            std::shared_lock lock(m_lock);
            if (auto const it = m_resource_map.find(uuid);
                it != m_resource_map.end())
            {
                return it->second.get();
            }
        }

        std::unique_lock lock(m_lock);
        auto const& [it, inserted] = m_resource_map.insert(std::pair(uuid, std::unique_ptr<resource>()));
        if (inserted)
        {
            if (auto const file_it = m_uuid_to_resource_file.find(uuid);
                file_it != m_uuid_to_resource_file.end())
            {
                gserializer::json_read_serializer serializer(file_it->second.c_str());
                serializer.process("resource", it->second, resource::factory());
                if (it->second)
                {
                    it->second->load();
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
                        m_paths.push_back(std::filesystem::absolute(file).make_preferred());
                        m_file_dependant_map[std::filesystem::hash_value(m_paths.back())].push_back(it->second->get_uuid());
                    }
                }
            }
        }
        return it->second.get();
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
                        it->second->load();
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
                                it->second->load();
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
                        it->second->load();
                    }
                }
            }
        }
        m_file_changes.clear();
    }
}
