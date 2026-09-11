#pragma once

#include "gcore/script/node.h"
#include "gcore/script/script_context.h"

#include <glm/glm.hpp>

namespace grender
{
    struct get_input_frame_data_node : gcore::node 
    {
        GCORE_DECLARE_NODE_TYPE(get_input_frame_data_node);
        bool is_const() const override { return false; }
        bool is_pure() const override { return false; }
        bool is_root() const override { return false; }

        using out_transform_matrix_pin = gcore::output_pin_descriptor<glm::mat4, 0, 1>;
        using out_view_matrix_pin = gcore::output_pin_descriptor<glm::mat4, 1, 2>;
        using out_projection_matrix_pin = gcore::output_pin_descriptor<glm::mat4, 2, 3>;

        void execute(gcore::node_context& context) const override;

        gcore::node::pin_descriptors get_pin_descriptors() const override;
    };
}