#include "stdafx.h"
#include "gcore/resource_dependency_tracker.h"
#include "gcore/resource.h"
#include "gcore/serializers/dependency_gatherer_serializer.h"

namespace gcore
{
    void resource_dependency_tracker::on_resource_loaded(resource& res)
    {
        gcore::dependency_gatherer_serializer dependency;
        dependency.process("resource", res);

        add_dependencies(res.get_uuid(), dependency.m_uuids, dependency.m_files);
    }

    void resource_dependency_tracker::add_dependencies(gtl::uuid const& resource_uuid, gtl::span<gtl::uuid const> uuids, gtl::span<std::filesystem::path const> files)
    {
        std::unique_lock lock(m_lock);
        for (auto const& uuid : uuids)
        {
            if (uuid != resource_uuid)
            {
                auto& dependant_vector = m_uuid_dependant_map[uuid];
                if (auto it = std::find(dependant_vector.begin(), dependant_vector.end(), resource_uuid);
                    it == dependant_vector.end())
                {
                    dependant_vector.push_back(resource_uuid);
                }
            }
        }

        for (auto const& file : files)
        {
            auto& dependant_vector = m_file_dependant_map[std::filesystem::hash_value(std::filesystem::absolute(file).make_preferred())];
            if (auto it = std::find(dependant_vector.begin(), dependant_vector.end(), resource_uuid);
                it == dependant_vector.end())
            {
                dependant_vector.push_back(resource_uuid);
            }
        }
    }

    gtl::span<gtl::uuid const> resource_dependency_tracker::get_uuid_depending_on_uuid(gtl::uuid const& uuid) const
    {
        auto const it = m_uuid_dependant_map.find(uuid);
        if (it != m_uuid_dependant_map.end())
            return it->second;
        return gtl::span<gtl::uuid const>();
    }

    gtl::span<gtl::uuid const> resource_dependency_tracker::get_uuid_depending_on_file(std::filesystem::path const& path) const
    {
        auto const it = m_file_dependant_map.find(std::filesystem::hash_value(std::filesystem::absolute(path).make_preferred()));
        if (it != m_file_dependant_map.end())
            return it->second;

        return gtl::span<gtl::uuid const>();
    }
}
