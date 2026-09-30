#pragma once

#include "gcore/script/node.h"
#include "gcore/script/script_context.h"
#include "gcore/script/execution_defines.h"

namespace gcore
{
    struct variable_node : node
    {
        using super = node;

        bool is_const() const override { return false; }

        void pre_create(script_descriptor& descriptor) override;
        void process(gserializer::serializer& serializer) override;
    protected:
        gtl::uuid m_variable_id;
        mutable pin_descriptor m_pin_descriptor;
        node_data_type::id_type m_type_id = 0;
    };

    struct get_variable_node : variable_node
    {
        using super = variable_node;
        GCORE_DECLARE_NODE_TYPE(get_variable_node);

        void execute(node_context& context) const override;
        pin_descriptors get_pin_descriptors() const override;
    };

    struct set_variable_node : variable_node
    {
        GCORE_DECLARE_NODE_TYPE(set_variable_node);
        using super = variable_node;
        using output_pin_descriptor = output_pin_descriptor<execution_pin_data, 0, 2>;
        void execute(node_context& context) const override;
        pin_descriptors get_pin_descriptors() const override;
    };
}
