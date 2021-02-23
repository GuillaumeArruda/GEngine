#pragma once

#include "gcore/component.h"

#include "gtl/uuid.h"

#include "grender/resources/program.h"
#include "grender/resources/mesh_resource.h"

namespace grender
{
    struct skybox_component : gcore::component
    {
        void process(gserializer::serializer& serializer) override;

        gcore::resource_handle<program> m_program;
        gcore::resource_handle<mesh_resource> m_mesh;
        program_uniform_state m_program_state;
    private:
        GSERIALIZER_DECLARE_SUBCLASS_FACTORY_REGISTRATION();
    };
}