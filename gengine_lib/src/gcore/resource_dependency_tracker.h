#pragma once
#include <unordered_map>
#include <filesystem>

#include "gtl/uuid.h"
#include "gtl/span.h"

namespace gcore
{
    struct resource;
    struct resource_dependency_tracker
    {

        void on_resource_loaded(resource& res);

        gtl::span<gtl::uuid const> get_uuid_depending_on_uuid(gtl::uuid const& uuid) const;
        gtl::span<gtl::uuid const> get_uuid_depending_on_file(std::filesystem::path const& path) const;

    private:
        std::unordered_map<gtl::uuid, std::vector<gtl::uuid>> m_uuid_depending_map;
        std::unordered_map<gtl::uuid, std::vector<std::filesystem::path>> m_file_depending_map;
        std::unordered_map<gtl::uuid, std::vector<gtl::uuid>> m_uuid_dependant_map;
        std::unordered_map<std::size_t, std::vector<gtl::uuid>> m_file_dependant_map;
    };
}