#pragma once

#include "gcore/system.h"
#include "gtl/callbacks.h"
#include "gcore/gcore_defines.h"

namespace gcore
{
    struct transform_component;
    struct input_component;
    struct flying_controller_component;
    struct world;
    struct flying_controller_system : system
    {
        void connect_to_world(gcore::world& world) override;
        void disconnect_from_world(gcore::world& world) override;
        void update(world& registry);
        void on_flying_controller_added(std::tuple<entity, transform_component*, input_component*, flying_controller_component*>& added_entity);
    
        gtl::callback_id m_on_added_callback_id = 0;
    };
}