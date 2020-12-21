#include "stdafx.h"

#include "grender/components/skybox_component.h"

#include "gserializer/serializer.h"

GSERIALIZER_DEFINE_SUBCLASS_FACTORY_REGISTRATION(grender::skybox_component);

namespace grender
{
    void skybox_component::process(gserializer::serializer& serializer)
    {
        serializer.process("program_id", m_program_uuid, m_program);
        serializer.process("mesh_id", m_mesh_uuid, m_mesh);
        serializer.process("program_state", m_program_state);
    }
}

