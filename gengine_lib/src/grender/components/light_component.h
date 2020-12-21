#pragma once

#include "gcore/component.h"

#include "grender/resources/program.h"
#include "grender/resources/mesh_resource.h"

namespace grender
{
    struct light_component : gcore::component
    {
        void process(gserializer::serializer& serializer) override;

        gtl::uuid m_main_program_uuid;
        gtl::uuid m_stencil_program_uuid;
        gtl::uuid m_mesh_uuid;
        gcore::resource_handle<program> m_main_program;
        gcore::resource_handle<program> m_stencil_program;
        gcore::resource_handle<mesh_resource> m_mesh;
        program_uniform_state m_main_state;
        program_uniform_state m_stencil_state;

        GSERIALIZER_DECLARE_SUBCLASS_FACTORY_REGISTRATION();
    };
}