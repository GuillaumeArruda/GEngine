#pragma once

#include "gcore/script/node_helper.h"

#include <glm/glm.hpp>

namespace gcore
{
    template<class MatrixType>
    struct transpose_matrix_node : node_helper_1_in_1_out<transpose_matrix_node<MatrixType>, MatrixType, MatrixType>
    {
        GCORE_DECLARE_NODE_TYPE(transpose_matrix_node);
        using super = node_helper_1_in_1_out<transpose_matrix_node<MatrixType>, MatrixType, MatrixType>;

        bool is_const() const override { return false; }
        bool is_pure() const override { return true; }

        void execute_on_element(node_context&, MatrixType const& input, MatrixType& out) const
        {
            out = glm::transpose(input);
        }

        node::pin_descriptors get_pin_descriptors() const override
        {
            static pin_descriptor const input_descriptors[] = {
                super::in_pin::get("input"),
            };
            static pin_descriptor const output_descriptors[] = {
                super::out_pin::get("result"),
            };
            return { input_descriptors,  output_descriptors };
        }
    };

    template<class MatrixType>
    struct inverse_matrix_node : node_helper_1_in_1_out<inverse_matrix_node<MatrixType>, MatrixType, MatrixType>
    {
        GCORE_DECLARE_NODE_TYPE(inverse_matrix_node);
        using super = node_helper_1_in_1_out<inverse_matrix_node<MatrixType>, MatrixType, MatrixType>;

        bool is_const() const override { return false; }
        bool is_pure() const override { return true; }

        void execute_on_element(node_context&, MatrixType const& input, MatrixType& out) const
        {
            out = glm::inverse(input);
        }

        node::pin_descriptors get_pin_descriptors() const override
        {
            static pin_descriptor const input_descriptors[] = {
                super::in_pin::get("input"),
            };
            static pin_descriptor const output_descriptors[] = {
                super::out_pin::get("result"),
            };
            return { input_descriptors,  output_descriptors };
        }
    };

    struct make_mat4_node : node_helper_3_in_1_out<make_mat4_node, glm::vec3, glm::vec3, glm::vec3, glm::mat4>
    {
        GCORE_DECLARE_NODE_TYPE(make_mat4_node);
        using super = node_helper_3_in_1_out<make_mat4_node, glm::vec3, glm::vec3, glm::vec3, glm::mat4>;

        bool is_const() const override { return false; }
        bool is_pure() const override { return true; }

        void execute_on_element(node_context&, glm::vec3 const& translation, glm::vec3 const& rotation, glm::vec3 const& scale, glm::mat4& out) const;
        node::pin_descriptors get_pin_descriptors() const override;
    };

    struct break_mat4_node : node
    {
        GCORE_DECLARE_NODE_TYPE(break_mat4_node);

        using in_matrix_pin = input_pin_descriptor<glm::mat4, 0, 1>;
        
        using out_translation_pin = output_pin_descriptor<glm::vec3, 0, 100>;
        using out_rotation_pin = output_pin_descriptor<glm::vec3, 1, 101>;
        using out_scale_pin = output_pin_descriptor<glm::vec3, 2, 102>;

        bool is_const() const override { return false; }
        bool is_pure() const override { return true; }

        void execute(node_context& context) const override;
        node::pin_descriptors get_pin_descriptors() const override;
    };
}