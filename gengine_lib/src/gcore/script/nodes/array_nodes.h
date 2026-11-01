#pragma once

#include "gcore/script/node.h"
#include "gcore/script/script_context.h"

namespace gcore
{
    struct array_size_node : node
    {
        GCORE_DECLARE_NODE_TYPE(array_size_node);

        using output = output_pin_descriptor<int, 0>;

        bool is_const() const override { return false; }
        bool is_pure() const override { return true; }

        void execute(node_context& context) const override;
        pin_descriptors get_pin_descriptors() const override;
    };
}