#pragma once

#include "gcore/script/node.h"
#include "gcore/script/node_data.h"
#include "gcore/script/script_context.h"

namespace gcore
{
    struct constant_node : node
    {
        GCORE_DECLARE_NODE_TYPE(constant_node);
        enum
        {
            output_id = 1
        };
        enum
        {
            output_index,
        };

        void execute(node_context& context) const override;
        node::pin_descriptors get_pin_descriptors() const override;

        node_data& get_node_data() { return m_data; }
    private:
        node_data m_data;
        mutable pin_descriptor m_pin_descriptor;
    };
}