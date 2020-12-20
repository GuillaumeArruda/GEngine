#pragma once

#include "gcore/component.h"

#include "grender/resources/program.h"

namespace grender
{
    struct light_component : gcore::component
    {
        void process(gserializer::serializer& serializer) override;

        gtl::uuid m_main_program;
        gtl::uuid m_stencil_program;
        gtl::uuid m_mesh;
        program_uniform_state m_main_state;
        program_uniform_state m_stencil_state;

        GSERIALIZER_DECLARE_SUBCLASS_FACTORY_REGISTRATION();
    };
}