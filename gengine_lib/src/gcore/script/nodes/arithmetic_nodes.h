#pragma once

#include "gcore/script/node_helper.h"

namespace gcore
{
    template<class T>
    struct add_node : node_helper_2_in_1_out<add_node<T>, T, T, T>
    {
        GCORE_DECLARE_NODE_TYPE(add_node);
        using super = node_helper_2_in_1_out<add_node<T>, T, T, T>;

        bool is_const() const override { return false; }
        bool is_pure() const override { return true; }

        void execute_on_element(node_context&, T const& left, T const& right, T& out) const
        {
            out = left + right;
        }

        node::pin_descriptors get_pin_descriptors() const override
        {
            static pin_descriptor const input_descriptors[] = {
                super::in_pin_1::get("left"),
                super::in_pin_2::get("right"),
            };
            static pin_descriptor const output_descriptors[] = {
                super::out_pin::get("result"),
            };
            return { input_descriptors,  output_descriptors };
        }
    };

    template<class T>
    struct substract_node : node_helper_2_in_1_out<substract_node<T>, T, T, T>
    {
        GCORE_DECLARE_NODE_TYPE(substract_node);
        using super = node_helper_2_in_1_out<substract_node<T>, T, T, T>;

        bool is_const() const override { return false; }
        bool is_pure() const override { return true; }

        void execute_on_element(node_context&, T const& left, T const& right, T& out) const
        {
            out = left - right;
        }

        node::pin_descriptors get_pin_descriptors() const override
        {
            static pin_descriptor const input_descriptors[] = {
                super::in_pin_1::get("left"),
                super::in_pin_2::get("right"),
            };
            static pin_descriptor const output_descriptors[] = {
                super::out_pin::get("result"),
            };
            return { input_descriptors,  output_descriptors };
        }
    };

    template<class T>
    struct multiply_node : node_helper_2_in_1_out<multiply_node<T>, T, T, T>
    {
        GCORE_DECLARE_NODE_TYPE(multiply_node);
        using super = node_helper_2_in_1_out<multiply_node<T>, T, T, T>;

        bool is_const() const override { return false; }
        bool is_pure() const override { return true; }

        void execute_on_element(node_context&, T const& left, T const& right, T& out) const
        {
            out = left * right;
        }

        node::pin_descriptors get_pin_descriptors() const override
        {
            static pin_descriptor const input_descriptors[] = {
                super::in_pin_1::get("left"),
                super::in_pin_2::get("right"),
            };
            static pin_descriptor const output_descriptors[] = {
                super::out_pin::get("result"),
            };
            return { input_descriptors,  output_descriptors };
        }
    };

    template<class T>
    struct divide_node : node_helper_2_in_1_out<divide_node<T>, T, T, T>
    {
        GCORE_DECLARE_NODE_TYPE(divide_node);
        using super = node_helper_2_in_1_out<divide_node<T>, T, T, T>;

        bool is_const() const override { return false; }
        bool is_pure() const override { return true; }

        void execute_on_element(node_context&, T const& left, T const& right, T& out) const
        {
            out = left / right;
        }

        node::pin_descriptors get_pin_descriptors() const override
        {
            static pin_descriptor const input_descriptors[] = {
                super::in_pin_1::get("left"),
                super::in_pin_2::get("right"),
            };
            static pin_descriptor const output_descriptors[] = {
                super::out_pin::get("result"),
            };
            return { input_descriptors,  output_descriptors };
        }
    };

#define GCORE_REGISTER_ARITHMETIC_NODES(Type, DisplayName)                                                                  \
    factory.register_type<gcore::add_node<Type>>("gcore::add_node<"#Type">", "Add ("DisplayName")");                        \
    factory.register_type<gcore::substract_node<Type>>("gcore::subtract_node<"#Type">", "Subtract ("DisplayName")");        \
    factory.register_type<gcore::multiply_node<Type>>("gcore::multiply_node<"#Type">", "Multiply ("DisplayName")");         \
    factory.register_type<gcore::divide_node<Type>>("gcore::divide_node<"#Type">", "Divide ("DisplayName")");         
}