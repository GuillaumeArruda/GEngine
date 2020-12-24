#include "stdafx.h"
#include "gcore/system_registry.h"

namespace gcore
{
    system_registry::~system_registry() = default;
    system_registry::system_registry(system_registry&&) noexcept = default;
    system_registry& system_registry::operator=(system_registry&&) noexcept = default;

    void gcore::system_registry::add_system(std::unique_ptr<system> system)
    {
        m_system_map.insert(std::pair(system_id(typeid(*system.get())), std::move(system)));
    }

    void system_registry::connect_systems_to_world(gcore::world& world)
    {
        for (auto& system : m_system_map)
        {
            system.second->connect_to_world(world);
        }
    }

}
