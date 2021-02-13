#pragma once

#include "gcore/script/node.h"
#include "gcore/script/node_helper.h"
#include "gcore/script/script_context.h"

namespace gcore
{
    template<class T>
    struct equal_node : node_helper_2_in_1_out<equal_node<T>, T, T, bool>
    {
        GCORE_DECLARE_NODE_TYPE(equal_node);
        using super = node_helper_2_in_1_out<equal_node<T>, T, T, bool>;

        bool is_const() const override { return false; }
        bool is_pure() const override { return true; }

        void execute_on_element(node_context&, T const& left, T const& right, bool& out) const
        {
            out = left == right;
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
    struct not_equal_node : node_helper_2_in_1_out<not_equal_node<T>, T, T, bool>
    {
        GCORE_DECLARE_NODE_TYPE(not_equal_node);
        using super = node_helper_2_in_1_out<not_equal_node<T>, T, T, bool>;

        bool is_const() const override { return false; }
        bool is_pure() const override { return true; }

        void execute_on_element(node_context&, T const& left, T const& right, bool& out) const
        {
            out = left != right;
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
    struct less_node : node_helper_2_in_1_out<less_node<T>, T, T, bool>
    {
        GCORE_DECLARE_NODE_TYPE(less_node);
        using super = node_helper_2_in_1_out<less_node<T>, T, T, bool>;

        bool is_const() const override { return false; }
        bool is_pure() const override { return true; }

        void execute_on_element(node_context&, T const& left, T const& right, bool& out) const
        {
            out = left < right;
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
    struct less_or_equal_node : node_helper_2_in_1_out<less_or_equal_node<T>, T, T, bool>
    {
        GCORE_DECLARE_NODE_TYPE(less_or_equal_node);
        using super = node_helper_2_in_1_out<less_or_equal_node<T>, T, T, bool>;
        
        bool is_const() const override { return false; }
        bool is_pure() const override { return true; }

        void execute_on_element(node_context&, T const& left, T const& right, bool& out) const
        {
            out = left <= right;
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
    struct greater_node : node_helper_2_in_1_out<greater_node<T>, T, T, bool>
    {
        GCORE_DECLARE_NODE_TYPE(greater_node);
        using super = node_helper_2_in_1_out<greater_node<T>, T, T, bool>;

        bool is_const() const override { return false; }
        bool is_pure() const override { return true; }

        void execute_on_element(node_context&, T const& left, T const& right, bool& out) const
        {
            out = left > right;
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
    struct greater_or_equal_node : node_helper_2_in_1_out<greater_or_equal_node<T>, T, T, bool>
    {
        GCORE_DECLARE_NODE_TYPE(greater_or_equal_node);
        using super = node_helper_2_in_1_out<greater_or_equal_node<T>, T, T, bool>;

        bool is_const() const override { return false; }
        bool is_pure() const override { return true; }

        void execute_on_element(node_context&, T const& left, T const& right, bool& out) const
        {
            out = left >= right;
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

#define GCORE_REGISTER_EQUALITY_NODES(Type, DisplayName)                                                                                    \
                factory.register_type<gcore::equal_node<Type>>("gcore::equal_node<"#Type">", "Equal ("DisplayName")");                      \
                factory.register_type<gcore::not_equal_node<Type>>("gcore::not_equal_node<"#Type">", "Not Equal ("DisplayName")");

#define GCORE_REGISTER_COMPARISON_NODES(Type, DisplayName)                                                                                  \
    GCORE_REGISTER_EQUALITY_NODES(Type, DisplayName)                                                                                        \
    factory.register_type<gcore::less_node<Type>>("gcore::less_node<"#Type">", "Less ("DisplayName")");                                     \
    factory.register_type<gcore::less_or_equal_node<Type>>("gcore::less_or_equal_node<"#Type">", "Less Or Equal ("DisplayName")");          \
    factory.register_type<gcore::greater_node<Type>>("gcore::greater_node<"#Type">", "Greater ("DisplayName")");                            \
    factory.register_type<gcore::greater_or_equal_node<Type>>("gcore::greater_or_equal_node<"#Type">", "Greater Or Equal ("DisplayName")");
}