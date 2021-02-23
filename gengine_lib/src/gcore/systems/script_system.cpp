#include "stdafx.h"

#include "gcore/systems/script_system.h"
#include "gcore/world.h"
#include "gcore/components/script_component.h"

namespace gcore
{
    void script_system::update(world& world)
    {
        auto view = world.get_entity_registry().get_view<script_component>();
        for (auto& [entity, script_comp] : view)
        {
            for (script_component::script_info& script_info : script_comp->m_scripts)
            {
                if (script_info.m_script.is_loaded())
                {
                    if (!script_info.m_context.is_prepared())
                    {
                        script_info.m_context = script_info.m_script->create_context();
                        script_info.m_context.prepare();
                    }
                    script_info.m_context.execute();
                }
            }
        }
    }
}

