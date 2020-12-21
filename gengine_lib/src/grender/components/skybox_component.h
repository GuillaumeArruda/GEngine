#pragma once

#include "gcore/component.h"

#include "gtl/uuid.h"

#include "grender/resources/program.h"

namespace grender
{
    struct skybox_component : gcore::component
    {
        void process(gserializer::serializer& serializer) override;

        gtl::uuid m_program_id;
        gtl::uuid m_mesh_id;
        program_uniform_state m_program_state;
    private:
        GSERIALIZER_DECLARE_SUBCLASS_FACTORY_REGISTRATION();
    };
}