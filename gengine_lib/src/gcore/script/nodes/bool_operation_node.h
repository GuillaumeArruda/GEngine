#pragma once

#include "gcore/script/node_helper.h"

namespace gcore
{
    struct and_node : node_helper_2_in_1_out<and_node, bool, bool, bool>
    {
        GCORE_DECLARE_NODE_TYPE(and_node);
        using super = node_helper_2_in_1_out<and_node, bool, bool, bool>;

        bool is_const() const { return false; }
        bool is_pure() const override { return true; }

        void execute_on_element(node_context&, bool const& left, bool const& right, bool& out) const;

        node::pin_descriptors get_pin_descriptors() const override;
    };

    struct or_node : node_helper_2_in_1_out<or_node, bool, bool, bool>
    {
        GCORE_DECLARE_NODE_TYPE(or_node);
        using super = node_helper_2_in_1_out<or_node, bool, bool, bool>;

        bool is_const() const { return false; }
        bool is_pure() const override { return true; }

        void execute_on_element(node_context&, bool const& left, bool const& right, bool& out) const;

        node::pin_descriptors get_pin_descriptors() const override;
    };

    struct not_node : node_helper_1_in_1_out<not_node, bool, bool>
    {
        GCORE_DECLARE_NODE_TYPE(not_node);
        using super = node_helper_1_in_1_out<not_node, bool, bool>;

        bool is_const() const { return false; }
        bool is_pure() const override { return true; }

        void execute_on_element(node_context&, bool const& in, bool& out) const;

        node::pin_descriptors get_pin_descriptors() const override;
    };
}