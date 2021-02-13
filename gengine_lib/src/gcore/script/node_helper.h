#pragma once

#include "gcore/script/node.h"
#include "gcore/script/script_context.h"

namespace gcore
{
    template<class CRTP, class InType, class OutType, int InId1 = 0, int OutId = 100>
    struct node_helper_1_in_1_out : node
    {
        using in_pin = input_pin_descriptor<InType, 0, InId1>;
        using out_pin = output_pin_descriptor<OutType, 0, OutId>;

        void execute(node_context& context) const override
        {
            auto const input = context.read<in_pin>();
            auto output = context.create_array<out_pin>(input.size());
            for (std::size_t i = 0; i < output.size(); ++i)
            {
                static_cast<CRTP const*>(this)->execute_on_element(context, input[i], output[i]);
            }
        }
    };

    template<class CRTP, class InType1, class InType2, class OutType, int InId1 = 0, int InId2 = 1, int OutId = 100>
    struct node_helper_2_in_1_out : node
    {
        using in_pin_1 = input_pin_descriptor<InType1, 0, InId1>;
        using in_pin_2 = input_pin_descriptor<InType2, 1, InId2>;

        using out_pin = output_pin_descriptor<OutType, 0, OutId>;

        void execute(node_context& context) const override
        {
            auto const input_1 = context.read<in_pin_1>();
            auto const input_2 = context.read<in_pin_2>();

            std::size_t const size_1 = input_1.size();
            std::size_t const size_2 = input_2.size();

            if (size_1 != size_2 && (size_1 != 1 || size_2 != 1) || size_1 == 0 || size_2 == 0)
            {
                context.create_array<out_pin>(0);
                return;
            }

            std::size_t const output_size = std::max(size_1, size_2);
            auto output = context.create_array<out_pin>(output_size);
            for (std::size_t i = 0; i < output.size(); ++i)
            {
                static_cast<CRTP const*>(this)->execute_on_element(context, size_1 == 1 ? input_1[0] : input_1[i], size_2 == 1 ? input_2[0] : input_2[i], output[i]);
            }
        }
    };
}