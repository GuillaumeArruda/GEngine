#pragma once

#include "gtl/uuid.h"

#include "gcore/component.h"
#include "gcore/resource_handle.h"

#include "grender/resources/program.h"
#include "grender/resources/mesh_resource.h"

namespace grender
{
    struct graphic_component : gcore::component
    {
        struct mesh_info
        {
            gtl::uuid m_mesh_id;
            gtl::uuid m_program_id;
            gcore::resource_handle<mesh_resource> m_mesh;
            gcore::resource_handle<program> m_program;
            program_uniform_state m_uniform_state;
            void process(gserializer::serializer& serializer);
        };

        void process(gserializer::serializer& serializer) override;

        std::vector<mesh_info> m_meshes;
        GSERIALIZER_DECLARE_SUBCLASS_FACTORY_REGISTRATION();
    };
}