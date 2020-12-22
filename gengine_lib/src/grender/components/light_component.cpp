#include "stdafx.h"
#include "grender/components/light_component.h"

#include "gserializer/serializer.h"

GSERIALIZER_DEFINE_SUBCLASS_FACTORY_REGISTRATION(grender::light_component);

namespace grender
{
    void light_component::process(gserializer::serializer& serializer)
    {
        serializer.process("main_program", m_main_program_uuid, m_main_program);
        serializer.process("mesh", m_mesh_uuid, m_mesh);
        serializer.process("main_state", m_main_state);
    }
}

