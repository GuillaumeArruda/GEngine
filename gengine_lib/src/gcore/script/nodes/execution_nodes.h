#pragma once

#include "gcore/script/node.h"
#include "gcore/script/execution_defines.h"
#include "gcore/script/script_context.h"

namespace gcore
{
    struct root_execution_node : node
    {
        GCORE_DECLARE_NODE_TYPE(root_execution_node);

        using in_pin = input_pin_descriptor<execution_pin_data, 0, 0>;

        bool is_const() const override { return false; }
        bool is_pure() const override { return false; }
        bool is_root() const override { return true; }

        void execute(node_context& context) const override;

        node::pin_descriptors get_pin_descriptors() const override;
    };
}