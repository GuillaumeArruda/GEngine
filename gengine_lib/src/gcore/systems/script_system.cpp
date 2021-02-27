#include "stdafx.h"

#include "gcore/systems/script_system.h"
#include "gcore/world.h"
#include "gcore/console.h"
#include "gcore/components/script_component.h"

namespace gcore
{
    GCORE_REGISTER_CONSOLE_VALUE("script.enable", bool, script_enable, true);

    void script_system::connect_to_world(gcore::world& world)
    {
        auto view = world.get_entity_registry().get_view<script_component>();
        view.add_on_added_callback([&](std::tuple<entity, script_component*> added_entity) {this->on_added_script_entity(added_entity); });
    }

    void script_system::update(world& world)
    {
        if (!script_enable)
            return;

        auto view = world.get_entity_registry().get_view<script_component>();
        for (auto& [entity, script_comp] : view)
        {
            for (script_component::script_info& script_info : script_comp->m_scripts)
            {
                if (script_info.m_context.is_prepared())
                    script_info.m_context.execute();
            }
        }
    }

    void script_system::on_added_script_entity(std::tuple<entity, script_component*>& added_entity)
    {
        for (script_component::script_info& script_info : std::get<script_component*>(added_entity)->m_scripts)
        {
            if (script_info.m_script.is_loaded())
            {
                script_info.m_context = script_info.m_script->create_context();
                script_info.m_context.prepare();
            }
        }
    }
}

