#pragma once

#include "gcore/entity_registry.h"
#include "gcore/system_registry.h"

namespace gcore
{
    struct system;
    struct world
    {
        entity_registry& get_entity_registry() { return m_entity_registry; }
        entity_registry const& get_entity_registry() const { return m_entity_registry; }

        system_registry& get_system_registry() { return m_system_registry; }
        system_registry const& get_system_registry() const { return m_system_registry; }
    
    private:
        entity_registry m_entity_registry;
        system_registry m_system_registry;
    };
}

