#pragma once

#include <unordered_map>

#include "gtl/span.h"

#include "gcore/gcore_defines.h"
#include "gcore/resource_handle.h"

namespace gcore
{
    struct entity_dependency_tracker
    {
        struct entity_component
        {
            entity m_entity;
            component_id m_component;
            constexpr auto operator<=>(entity_component const&) const = default;
        };

        void add_dependencies(entity ent, component_id comp_id, gtl::span<resource_handle<resource> const> resources);
        void remove_entity(entity ent);
        void clear();
        
        gtl::span<entity_component const> get_dependant_components(gtl::uuid resource_uuid) const;

    private:
        std::unordered_map<gtl::uuid, std::vector<entity_component>> m_resource_to_dependant_component;
        std::unordered_map<entity, std::vector<gtl::uuid>> m_entity_to_resources;
    };
}