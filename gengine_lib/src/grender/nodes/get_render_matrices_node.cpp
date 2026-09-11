#include "stdafx.h"

#include "grender/nodes/get_render_matrices_node.h"
#include "grender/systems/render_system.h"

#include "gcore/script/script_context.h"

namespace grender
{
    void get_render_matrices_node::execute(gcore::node_context& context) const
    {
        if (render_system const** render_sys = context.get_in_context<render_system const*>())
        {
            context.write_values<out_mvp_pin>((*render_sys)->get_mvp());
            context.write_values<out_normal_matrix_pin>((*render_sys)->get_normal_matrix());
            context.write_values<out_world_matrix_pin>((*render_sys)->get_transform());
        }
    }

    gcore::node::pin_descriptors get_render_matrices_node::get_pin_descriptors() const
    {
        static gcore::pin_descriptor const outputs[] = {
            out_mvp_pin::get("mvp"),
            out_normal_matrix_pin::get("normal_matrix"),
            out_world_matrix_pin::get("world matrix"),
        };
        return { gtl::span<gcore::pin_descriptor const>{}, outputs };
    }
}
