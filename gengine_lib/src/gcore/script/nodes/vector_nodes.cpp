#include <stdafx.h>

#include "gcore/script/nodes/vector_nodes.h"

namespace gcore
{
    void make_vec3_node::execute_on_element(node_context&, float x, float y, float z, glm::vec3& out) const
    {
        out = glm::vec3(x, y, z);
    }

    node::pin_descriptors make_vec3_node::get_pin_descriptors() const
    {
        static pin_descriptor const input_descriptors[] = {
            super::in_pin_1::get("x"),
            super::in_pin_2::get("y"),
            super::in_pin_3::get("z"),
        };
        static pin_descriptor const output_descriptors[] = {
            super::out_pin::get("vector"),
        };
        return node::pin_descriptors{ input_descriptors, output_descriptors };
    }

    void make_vec4_node::execute_on_element(node_context&, float x, float y, float z, float w, glm::vec4& out) const
    {
        out = glm::vec4(x, y, z, w);
    }

    node::pin_descriptors make_vec4_node::get_pin_descriptors() const
    {
        static pin_descriptor const input_descriptors[] = {
            super::in_pin_1::get("x"),
            super::in_pin_2::get("y"),
            super::in_pin_3::get("z"),
            super::in_pin_4::get("w"),
        };
        static pin_descriptor const output_descriptors[] = {
            super::out_pin::get("vector"),
        };
        return node::pin_descriptors{ input_descriptors, output_descriptors };
    }

    void break_vec3_node::execute(node_context& context) const
    {
        gtl::span<glm::vec3 const> inputs = context.read<in_vector_pin>();
        gtl::span<float> x = context.create_array<out_x_pin>(inputs.size());
        gtl::span<float> y = context.create_array<out_y_pin>(inputs.size());
        gtl::span<float> z = context.create_array<out_z_pin>(inputs.size());

        for (std::size_t i = 0; i < inputs.size(); ++i)
        {
            x[i] = inputs[i].x;
            y[i] = inputs[i].y;
            z[i] = inputs[i].z;
        }
    }
    node::pin_descriptors break_vec3_node::get_pin_descriptors() const
    {
        static pin_descriptor const input_descriptors[] = {
           in_vector_pin::get("vector"),
        };
        static pin_descriptor const output_descriptors[] = {
            out_x_pin::get("x"),
            out_y_pin::get("y"),
            out_z_pin::get("z"),
        };

        return node::pin_descriptors{ input_descriptors, output_descriptors };
    }

    void break_vec4_node::execute(node_context& context) const
    {
        gtl::span<glm::vec4 const> inputs = context.read<in_vector_pin>();
        gtl::span<float> x = context.create_array<out_x_pin>(inputs.size());
        gtl::span<float> y = context.create_array<out_y_pin>(inputs.size());
        gtl::span<float> z = context.create_array<out_z_pin>(inputs.size());
        gtl::span<float> w = context.create_array<out_w_pin>(inputs.size());

        for (std::size_t i = 0; i < inputs.size(); ++i)
        {
            x[i] = inputs[i].x;
            y[i] = inputs[i].y;
            z[i] = inputs[i].z;
            w[i] = inputs[i].w;
        }
    }

    node::pin_descriptors break_vec4_node::get_pin_descriptors() const
    {
        static pin_descriptor const input_descriptors[] = {
           in_vector_pin::get("vector"),
        };
        static pin_descriptor const output_descriptors[] = {
            out_x_pin::get("x"),
            out_y_pin::get("y"),
            out_z_pin::get("z"),
            out_w_pin::get("w"),
        };

        return node::pin_descriptors{ input_descriptors, output_descriptors };
    }
}
