#pragma once

#include "gcore/script/node_helper.h"
#include "gcore/script/execution_defines.h"

namespace gcore
{
    struct print_node : node
    {
        GCORE_DECLARE_NODE_TYPE(print_node);

        using in_pin = input_pin_descriptor<std::string, 0, 0>;
        using out_pin = output_pin_descriptor<execution_pin_data, 0, 100>;

        bool is_const() const override { return false; }
        bool is_pure() const override { return false; }

        void execute(node_context& context) const override;

        node::pin_descriptors get_pin_descriptors() const override;
    };
}