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
}