#include "stdafx.h"
#include "entity_dependency_tracker.h"


namespace gcore
{
    void entity_dependency_tracker::add_dependencies(entity ent, component_id comp_id, gtl::span<resource_handle<resource> const> resources)
    {
        std::vector<gtl::uuid>& dependant_resources = m_entity_to_resources[ent];
        for (resource_handle<resource> const& res : resources)
        {
            auto& res_vec = m_resource_to_dependant_component[res.get_uuid()];
            if (std::find(res_vec.begin(), res_vec.end(), entity_component{ ent, comp_id }) == res_vec.end())
                res_vec.push_back({ ent, comp_id });

            if (std::find(dependant_resources.begin(), dependant_resources.end(), res.get_uuid()) == dependant_resources.end())
            {
                dependant_resources.push_back(res.get_uuid());
            }
        }
    }

    void entity_dependency_tracker::remove_entity(entity ent)
    {
        auto it = m_entity_to_resources.find(ent);
        if (it == m_entity_to_resources.end())
            return;

        for (gtl::uuid const& res_uuid : it->second)
        {
            auto res_it = m_resource_to_dependant_component.find(res_uuid);
            if (res_it == m_resource_to_dependant_component.end())
                continue;

            std::erase_if(res_it->second, [&](entity_component const& entity_comp) { return entity_comp.m_entity == ent; });
        }

        m_entity_to_resources.erase(it);
    }

    void entity_dependency_tracker::clear()
    {
        m_resource_to_dependant_component.clear();
        m_entity_to_resources.clear();
    }

    gtl::span<entity_dependency_tracker::entity_component const> entity_dependency_tracker::get_dependant_components(gtl::uuid resource_uuid) const
    {
        auto it = m_resource_to_dependant_component.find(resource_uuid);
        if (it != m_resource_to_dependant_component.end())
        {
            return it->second;
        }

        return gtl::span<entity_component const>();
    }

}
