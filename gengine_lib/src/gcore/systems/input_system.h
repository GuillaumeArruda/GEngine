#pragma once

#include "gcore/system.h"

namespace gcore
{
    struct world;
    struct input_system : system
    {
        void update(world& world);
    };
}