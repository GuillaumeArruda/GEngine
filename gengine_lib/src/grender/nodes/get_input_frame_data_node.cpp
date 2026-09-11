#include "stdafx.h"

#include "grender/nodes/get_input_frame_data_node.h"
#include "grender/systems/render_system.h"
#include "grender/resources/graphic_script.h"

#include "gcore/script/script_context.h"

namespace grender
{
    void get_input_frame_data_node::execute(gcore::node_context& context) const
    {
        if (graphic_script_frame_input_data const* frame_input = context.get_in_context<graphic_script_frame_input_data const>())
        {
            context.write_values<out_transform_matrix_pin>(frame_input->m_transform);
            context.write_values<out_projection_matrix_pin>(frame_input->m_projection);
            context.write_values<out_view_matrix_pin>(frame_input->m_view_matrix);
        }
    }

    gcore::node::pin_descriptors get_input_frame_data_node::get_pin_descriptors() const
    {
        static gcore::pin_descriptor const outputs[] = {
            out_transform_matrix_pin::get("entity transform"),
            out_projection_matrix_pin::get("projection"),
            out_view_matrix_pin::get("view matrix"),
        };
        return { gtl::span<gcore::pin_descriptor const>{}, outputs };
    }
}
