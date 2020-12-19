#pragma once

#include <unordered_map>
#include <string>
#include <memory>
#include <shared_mutex>
#include <filesystem>

#include "gtl/uuid.h"

#include "gcore/resource.h"

namespace gserializer
{
    struct serializer;
}

namespace filewatch
{
    enum class event;
}

namespace gcore
{
    struct resource_library
    {
        void process(gserializer::serializer& serializer);

        template<class ResourceType>
        [[nodiscard]] ResourceType* get_resource(gtl::uuid const& uuid)
        {
            return dynamic_cast<ResourceType*>(get_resource(uuid));
        }

        std::string get_filepath(gtl::uuid const& uuid) const;

        void scan_directory(const char* folder);
        void on_file_change(std::wstring const& path);

        void update();

    private:
        resource* get_resource(gtl::uuid const& uuid);
        
        std::unordered_map<gtl::uuid, std::string> m_uuid_to_resource_file;
        std::unordered_map<gtl::uuid, std::unique_ptr<resource>> m_resource_map;
        std::unordered_map<gtl::uuid, std::vector<gtl::uuid>> m_uuid_dependant_map;
        std::unordered_map<std::size_t, std::vector<gtl::uuid>> m_file_dependant_map;
        std::vector<std::filesystem::path> m_paths;
        std::shared_mutex m_lock;
        std::mutex m_file_change_lock;
        std::vector<std::filesystem::path> m_file_changes;
    };
}