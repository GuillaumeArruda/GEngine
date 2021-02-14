#pragma once

#include "gcore/script/node_helper.h"

namespace gcore
{
    template<class Type>
    struct select_node : node_helper_3_in_1_out<select_node<Type>, bool, Type, Type, Type>
    {
        GCORE_DECLARE_NODE_TYPE(select_node);
        using super = node_helper_3_in_1_out<select_node<Type>, bool, Type, Type, Type>;

        bool is_const() const override { return false; }
        bool is_pure() const override { return true; }

        void execute_on_element(node_context&, bool const& condition, Type const& left, Type const& right, Type& out) const
        {
            out = condition ? left : right;
        }

        void execute(node_context& context) const override
        {
            if (!m_type_inputs_guaranteed_same_size)
            {
                super::execute(context);
                return;
            }

            gtl::span<bool const> conditions = context.read<super::in_pin_1>();
            bool has_true_output = false;
            bool has_false_output = false;
            for (bool condition : conditions)
            {
                if (condition)
                    has_true_output = true;
                else
                    has_false_output = true;
            }

            if (has_true_output && !has_false_output)
            {
                context.write_values<super::out_pin>(context.read<super::in_pin_2>());
            }
            else if (has_false_output && !has_true_output)
            {
                context.write_values<super::out_pin>(context.read<super::in_pin_3>());
            }
            else
            {
                super::execute(context);
            }

        }

        void process(gserializer::serializer& serializer)
        {
            super::process(serializer);
            serializer.process("type_inputs_guaranteed_same_size", m_type_inputs_guaranteed_same_size);
        }

        node::pin_descriptors get_pin_descriptors() const
        {
            static pin_descriptor const inputs[] = {
                super::in_pin_1::get("condition"),
                super::in_pin_2::get("true"),
                super::in_pin_3::get("false"), 
            };

            static pin_descriptor const outputs[] = {
                super::out_pin::get("result"),
            };
            return { inputs, outputs };
        }

    private:
        bool m_type_inputs_guaranteed_same_size = true;
    };
}