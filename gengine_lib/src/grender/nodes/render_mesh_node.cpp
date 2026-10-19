#include "stdafx.h"
#include "grender/nodes/render_mesh_node.h"

namespace grender
{
    void render_mesh_node::execute(gcore::node_context& context) const
    {
        if (!m_mesh.is_loaded())
            return;

        gtl::span<program_pin_data const> programs = context.read<in_program_pin>();
        for (auto& program : programs)
        {
            if (!program.m_program.is_loaded())
                continue;

            program.m_program->activate();
            program.m_state.apply();
            m_mesh->draw();
        }
    }

    void render_mesh_node::process(gserializer::serializer& serializer)
    {
        node::process(serializer);
        serializer.process("mesh", m_mesh);
    }

    gcore::node::pin_descriptors render_mesh_node::get_pin_descriptors() const
    {
        static gcore::pin_descriptor const inputs[] = {
            in_program_pin::get("program"), 
        };
        return { inputs, {} };
    }
}

