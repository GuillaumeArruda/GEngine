#pragma once

#include "gcore/script/node.h"
#include "gcore/script/node_helper.h"

#include <glm/glm.hpp>

namespace gcore
{
    struct make_vec3_node : node_helper_3_in_1_out<make_vec3_node, float, float, float, glm::vec3> 
    {
        GCORE_DECLARE_NODE_TYPE(make_vec3_node);
        using super = node_helper_3_in_1_out<make_vec3_node, float, float, float, glm::vec3>;
        
        bool is_const() const override { return false; }
        bool is_pure() const override { return true; }

        void execute_on_element(node_context&, float x, float y, float z, glm::vec3& out) const;
        node::pin_descriptors get_pin_descriptors() const override;
    };

    struct make_vec4_node : node_helper_4_in_1_out<make_vec4_node, float, float, float, float, glm::vec4>
    {
        GCORE_DECLARE_NODE_TYPE(make_vec4_node);
        using super = node_helper_4_in_1_out<make_vec4_node, float, float, float, float, glm::vec4>;
        
        bool is_const() const override { return false; }
        bool is_pure() const override { return true; }

        void execute_on_element(node_context&, float x, float y, float z, float w, glm::vec4& out) const;
        node::pin_descriptors get_pin_descriptors() const override;
    };

    struct break_vec3_node : gcore::node
    {
        GCORE_DECLARE_NODE_TYPE(break_vec3_node);

        using in_vector_pin = input_pin_descriptor<glm::vec3, 0, 0>;

        using out_x_pin = output_pin_descriptor<float, 0, 100>;
        using out_y_pin = output_pin_descriptor<float, 1, 101>;
        using out_z_pin = output_pin_descriptor<float, 2, 102>;

        bool is_const() const override { return false; }
        bool is_pure() const override { return true; }

        void execute(node_context& context) const override;
        node::pin_descriptors get_pin_descriptors() const override;
    };

    struct break_vec4_node : gcore::node
    {
        GCORE_DECLARE_NODE_TYPE(break_vec4_node);

        using in_vector_pin = input_pin_descriptor<glm::vec4, 0, 0>;

        using out_x_pin = output_pin_descriptor<float, 0, 100>;
        using out_y_pin = output_pin_descriptor<float, 1, 101>;
        using out_z_pin = output_pin_descriptor<float, 2, 102>;
        using out_w_pin = output_pin_descriptor<float, 3, 103>;

        bool is_const() const override { return false; }
        bool is_pure() const override { return true; }

        void execute(node_context& context) const override;
        node::pin_descriptors get_pin_descriptors() const override;
    };
}