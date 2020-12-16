#include "stdafx.h"
#include "gcore/system_registry.h"

namespace gcore
{
    system_registry::~system_registry() = default;
    system_registry::system_registry(system_registry&&) = default;
    system_registry& system_registry::operator=(system_registry&&) noexcept = default;

    void gcore::system_registry::add_system(std::unique_ptr<system> system)
    {
        m_system_map.insert(std::pair(typeid(system).hash_code(), std::move(system)));
    }

}
