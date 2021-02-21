#pragma once

#include "gcore/script/node.h"
#include "gcore/script/script_context.h"

#include "grender/resources/program.h"
#include "grender/resources/mesh_resource.h"

namespace grender
{
    struct render_mesh_node : gcore::node
    {
        GCORE_DECLARE_NODE_TYPE(render_mesh_node);
        bool is_const() const override { return false; }
        bool is_pure() const override { return false; }
        bool is_root() const override { return true; }

        using in_program_pin = gcore::input_pin_descriptor<program_pin_data, 0, 1>;
        using in_mesh_pin = gcore::input_pin_descriptor<gcore::resource_handle<mesh_resource>, 1, 0>;

        void execute(gcore::node_context& context) const override;

        node::pin_descriptors get_pin_descriptors() const override;
    };
}