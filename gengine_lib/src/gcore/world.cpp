#include "stdafx.h"

#include "gcore/world.h"
#include "gcore/system.h"

namespace gcore
{
    world::~world() = default;

    void world::initialize_systems()
    {
        m_system_registry.connect_systems_to_world(*this);
    }

    void world::deinitialize_systems()
    {
        m_system_registry.disconnect_systems_from_world(*this);
    }
}
