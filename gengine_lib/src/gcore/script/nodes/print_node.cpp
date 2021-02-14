#include "stdafx.h"

#include <iostream>

#include "gcore/script/nodes/print_node.h"

namespace gcore
{
    void print_node::execute(node_context& context) const
    {
        gtl::span<std::string const> messages = context.read<in_pin>();
        for (std::string const& message : messages)
        {
            std::cout << message << "\n";
        }
        context.create_array<out_pin>(1);
    }

    node::pin_descriptors print_node::get_pin_descriptors() const
    {
        static pin_descriptor const inputs = { in_pin::get("message") };
        static pin_descriptor const outputs = { out_pin::get("execute") };
        return { inputs, outputs };
    }

}

