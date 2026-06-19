#pragma once

#include "gcore/system.h"

namespace gcore
{
    struct world;
    struct script_component;
    struct script_system : system
    {
        void connect_to_world(gcore::world& world) override;
        void disconnect_from_world(gcore::world& world) override;

        void update(world& world);

        void on_added_script_entity(std::tuple<entity, script_component*>& added_entity);
        
    protected:
        gtl::callback_id m_on_added_callback_id = 0;
    };
}