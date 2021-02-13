#pragma once

#include "gcore/script/node_helper.h"

namespace gcore
{
    template<class From, class To>
    struct conversion_node : node_helper_1_in_1_out<conversion_node<From, To>, From, To>
    {
        using super = node_helper_1_in_1_out<conversion_node<From, To>, From, To>;
        GCORE_DECLARE_NODE_TYPE(conversion_node);
        bool is_const() const override { return false; }
        bool is_pure() const override { return true; }

        void execute_on_element(node_context&, From const& from, To& to) const
        {
            to = static_cast<To>(from);
        }

        node::pin_descriptors get_pin_descriptors() const
        {
            static pin_descriptor const input_descriptors[] = {
                super::in_pin::get("in"),
            };
            static pin_descriptor const output_descriptors[] = {
                super::out_pin::get("out"),
            };
            return { input_descriptors,  output_descriptors };
        }
    };
}