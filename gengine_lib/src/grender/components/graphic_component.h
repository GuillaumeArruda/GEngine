#pragma once

#include "gtl/uuid.h"

#include "gcore/component.h"

#include "grender/resources/program.h"

namespace grender
{
    struct mesh_resource;
    struct graphic_component : gcore::component
    {
        struct mesh_info
        {
            gtl::uuid m_mesh_id;
            gtl::uuid m_program_id;
            mesh_resource const* m_mesh;
            program const* m_program;
            program_uniform_state m_uniform_state;
            void process(gserializer::serializer& serializer);
        };

        void process(gserializer::serializer& serializer) override;

        std::vector<mesh_info> m_meshes;
        GSERIALIZER_DECLARE_SUBCLASS_FACTORY_REGISTRATION();
    };
}