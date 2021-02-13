#include "stdafx.h"
#include "gcore/script/nodes/bool_operation_node.h"

namespace gcore
{
    void and_node::execute_on_element(node_context&, bool const& left, bool const& right, bool& out) const
    {
        out = left && right;
    }

    node::pin_descriptors and_node::get_pin_descriptors() const
    {
        static pin_descriptor const input_descriptors[] = {
            super::in_pin_1::get("left"),
            super::in_pin_2::get("right"),
        };
        static pin_descriptor const output_descriptors[] = {
            super::out_pin::get("result"),
        };
        return { input_descriptors,  output_descriptors };
    }

    void or_node::execute_on_element(node_context&, bool const& left, bool const& right, bool& out) const
    {
        out = left || right;
    }

    node::pin_descriptors or_node::get_pin_descriptors() const
    {
        static pin_descriptor const input_descriptors[] = {
            super::in_pin_1::get("left"),
            super::in_pin_2::get("right"),
        };
        static pin_descriptor const output_descriptors[] = {
            super::out_pin::get("result"),
        };
        return { input_descriptors,  output_descriptors };
    }

    void not_node::execute_on_element(node_context&, bool const& in, bool& out) const
    {
        out = !in;
    }

    node::pin_descriptors not_node::get_pin_descriptors() const
    {
        static pin_descriptor const input_descriptors[] = {
            super::in_pin::get("in"),
        };
        static pin_descriptor const output_descriptors[] = {
            super::out_pin::get("out"),
        };
        return { input_descriptors,  output_descriptors };
    }
}


