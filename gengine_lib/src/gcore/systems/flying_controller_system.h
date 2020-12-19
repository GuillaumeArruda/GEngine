#pragma once

#include "gcore/system.h"

namespace gcore
{
    struct world;
    struct flying_controller_system : system
    {
        void update(world& registry);
    };
}