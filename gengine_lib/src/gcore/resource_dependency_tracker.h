#pragma once
#include <unordered_map>
#include <filesystem>
#include <mutex>

#include "gtl/uuid.h"
#include "gtl/span.h"

namespace gcore
{
    struct resource;
    struct resource_dependency_tracker
    {
        void on_resource_loaded(resource& res);

        void add_dependencies(gtl::uuid const& resource_uuid, gtl::span<gtl::uuid const> uuids, gtl::span<std::filesystem::path const> files);

        gtl::span<gtl::uuid const> get_uuid_depending_on_uuid(gtl::uuid const& uuid) const;
        gtl::span<gtl::uuid const> get_uuid_depending_on_file(std::filesystem::path const& path) const;

    private:
        std::mutex m_lock;
        std::unordered_map<gtl::uuid, std::vector<gtl::uuid>> m_uuid_dependant_map;
        std::unordered_map<std::size_t, std::vector<gtl::uuid>> m_file_dependant_map;
    };
}