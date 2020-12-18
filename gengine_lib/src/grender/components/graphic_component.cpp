#include "stdafx.h"

#include "gserializer/serializer.h"
#include "grender/components/graphic_component.h"

GSERIALIZER_DEFINE_SUBCLASS_FACTORY_REGISTRATION(grender::graphic_component);

namespace grender
{
    void graphic_component::process(gserializer::serializer& serializer)
    {
        serializer.process("meshes", m_meshes, "Mesh");
    }

    void graphic_component::mesh_info::process(gserializer::serializer& serializer)
    {
        serializer.process("mesh", m_mesh_id);
        serializer.process("program", m_program_id);
        serializer.process("uniform state", m_uniform_state);
    }
}

