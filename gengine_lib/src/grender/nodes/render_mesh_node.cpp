#include "stdafx.h"
#include "grender/nodes/render_mesh_node.h"

namespace grender
{
    void render_mesh_node::execute(gcore::node_context&) const
    {
        //
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

