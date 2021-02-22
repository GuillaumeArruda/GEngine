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

        for (auto const& uuid : dependency.m_uuids)
        {
            if (uuid != res.get_uuid())
            {
                auto& dependant_vector = m_uuid_dependant_map[uuid];
                if (auto it = std::find(dependant_vector.begin(), dependant_vector.end(), res.get_uuid());
                    it == dependant_vector.end())
                {
                    dependant_vector.push_back(res.get_uuid());
                }
            }
        }

        m_uuid_depending_map[res.get_uuid()] = std::move(dependency.m_uuids);

        for (auto const& file : dependency.m_files)
        {
            auto& dependant_vector = m_file_dependant_map[std::filesystem::hash_value(std::filesystem::absolute(file).make_preferred())];
            if (auto it = std::find(dependant_vector.begin(), dependant_vector.end(), res.get_uuid());
                it == dependant_vector.end())
            {
                dependant_vector.push_back(res.get_uuid());
            }
        }

        m_file_depending_map[res.get_uuid()] = std::move(dependency.m_files);
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
