#include "stdafx.h"
#include "grender/nodes/render_mesh_node.h"

namespace grender
{
    void render_mesh_node::execute(gcore::node_context& context) const
    {
        gtl::span<program_pin_data const> programs = context.read<in_program_pin>();
        gtl::span<gcore::resource_handle<mesh_resource> const> meshes = context.read<in_mesh_pin>();
        if (programs.size() != meshes.size() || programs.size() != 1 || meshes.size() != 1 || programs.size() == 0 || meshes.size() == 0)
        {
            return;
        }

        std::size_t const number_of_elements = std::max(programs.size(), meshes.size());
        for (std::size_t i = 0; i < number_of_elements; ++i)
        {
            program_pin_data const& program = programs.size() == 1 ? programs[0] : programs[i];
            gcore::resource_handle<mesh_resource> const& mesh = meshes.size() == 1 ? meshes[0] : meshes[i];
            if (!program.m_program.is_loaded() || !mesh.is_loaded())
                continue;

            program.m_program->activate();
            program.m_state.apply();
            mesh->draw();

         }

    }

    gcore::node::pin_descriptors render_mesh_node::get_pin_descriptors() const
    {
        static gcore::pin_descriptor const inputs[] = {
            in_program_pin::get("program"),
            in_mesh_pin::get("mesh"),
        };
        return { inputs, {} };
    }
}

