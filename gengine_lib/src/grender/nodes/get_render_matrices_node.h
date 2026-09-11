#pragma once

#include "gcore/script/node.h"
#include "gcore/script/script_context.h"

#include <glm/glm.hpp>

namespace grender
{
    struct get_render_matrices_node : gcore::node 
    {
        GCORE_DECLARE_NODE_TYPE(get_render_matrices_node);
        bool is_const() const override { return false; }
        bool is_pure() const override { return false; }
        bool is_root() const override { return false; }

        using out_mvp_pin = gcore::output_pin_descriptor<glm::mat4x4, 0, 1>;
        using out_normal_matrix_pin = gcore::output_pin_descriptor<glm::mat3x3, 1, 2>;
        using out_world_matrix_pin = gcore::output_pin_descriptor<glm::mat4x4, 2, 3>;

        void execute(gcore::node_context& context) const override;

        gcore::node::pin_descriptors get_pin_descriptors() const override;
    };
}