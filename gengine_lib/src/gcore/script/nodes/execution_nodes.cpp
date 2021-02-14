#include "stdafx.h"
#include "execution_nodes.h"

namespace gcore
{
    void root_execution_node::execute(node_context& context) const
    {
        context.read<in_pin>();
    }

    node::pin_descriptors root_execution_node::get_pin_descriptors() const
    {
        static pin_descriptor const inputs[] = { in_pin::get("execute") };
        return { inputs, {} };
    }
}
