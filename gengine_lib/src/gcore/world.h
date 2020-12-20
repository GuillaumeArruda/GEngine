#pragma once

#include "gcore/entity_registry.h"
#include "gcore/system_registry.h"
#include "gcore/resource_library.h"

namespace gcore
{
    struct system;
    struct world
    {
        world(std::shared_ptr<resource_library> library) : m_resource_library(std::move(library)) {}
        entity_registry& get_entity_registry() { return m_entity_registry; }
        entity_registry const& get_entity_registry() const { return m_entity_registry; }

        system_registry& get_system_registry() { return m_system_registry; }
        system_registry const& get_system_registry() const { return m_system_registry; }
    
        resource_library* get_resource_library() { return m_resource_library.get(); }
        resource_library const* get_resource_library() const { return m_resource_library.get();  }

    private:
        std::shared_ptr<resource_library> m_resource_library;
        entity_registry m_entity_registry;
        system_registry m_system_registry;
    };
}

