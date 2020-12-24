#include "stdafx.h"

#include "gcore/world.h"
#include "gcore/system.h"

namespace gcore
{
    void world::initialize_systems()
    {
        m_system_registry.connect_systems_to_world(*this);
    }
}
