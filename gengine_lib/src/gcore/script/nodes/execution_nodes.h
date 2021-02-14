#pragma once

#include "gcore/script/node.h"
#include "gcore/script/execution_defines.h"
#include "gcore/script/script_context.h"

namespace gcore
{
    struct root_execution_node : node
    {
        GCORE_DECLARE_NODE_TYPE(root_execution_node);

        root_execution_node();

        bool is_const() const override { return false; }
        bool is_pure() const override { return false; }
        bool is_root() const override { return true; }

        void execute(node_context& context) const override;

        node::pin_descriptors get_pin_descriptors() const override;
        void on_input_pin_state_changed(int pin_id, pin_state state) override;
        void process(gserializer::serializer& serializer) override;

    private:
        void fill_descriptors();
        std::vector<pin_descriptor> m_descriptors;
    };

    struct sequence_execution_node : node
    {
        GCORE_DECLARE_NODE_TYPE(sequence_execution_node);

        using out_pin = output_pin_descriptor<execution_pin_data, 0, 12000>;
        sequence_execution_node();

        bool is_const() const override { return false; }
        bool is_pure() const override { return false; }

        void execute(node_context& context) const override;

        node::pin_descriptors get_pin_descriptors() const override;
        void on_input_pin_state_changed(int pin_id, pin_state state) override;
        void process(gserializer::serializer& serializer) override;

    private:
        void fill_descriptors();
        std::vector<pin_descriptor> m_descriptors;
    };

    struct select_execution_node : node
    {
        GCORE_DECLARE_NODE_TYPE(select_execution_node);

        using condition_pin = input_pin_descriptor<bool, 0, 0>;
        using true_pin = input_pin_descriptor<execution_pin_data, 1, 1, true>;
        using false_pin = input_pin_descriptor<execution_pin_data, 2, 2, true>;

        using out_pin = output_pin_descriptor<execution_pin_data, 0, 100>;

        bool is_const() const override { return false; }
        bool is_pure() const override { return false; }

        void execute(node_context& context) const override;

        node::pin_descriptors get_pin_descriptors() const override;
    };
}