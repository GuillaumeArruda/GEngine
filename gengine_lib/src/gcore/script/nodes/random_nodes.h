#pragma once

#include "gcore/script/node_helper.h"

namespace gcore
{
    struct random_integer_range_node : node_helper_2_in_1_out<random_integer_range_node, int, int, int>
    {
        GCORE_DECLARE_NODE_TYPE(random_integer_range_node);
        using super = node_helper_2_in_1_out<random_integer_range_node, int, int, int>;

        bool is_const() const override { return false; }
        bool is_pure() const override { return false; }

        void execute_on_element(node_context&, int const& min, int const& max, int& value) const;
        node::pin_descriptors get_pin_descriptors() const override;
    };
}