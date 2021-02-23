#pragma once

#include "gcore/system.h"

namespace gcore
{
    struct world;
    struct script_system : system
    {
        void update(world& world);
    };
}