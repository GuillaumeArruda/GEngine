#pragma once

#include "gcore/system.h"

namespace gcore
{
    struct entity_registry;
    struct resource_library;
}

namespace grender
{
    struct render_system : gcore::system
    {
        void render(gcore::entity_registry& registry, gcore::resource_library& library);
    };
}