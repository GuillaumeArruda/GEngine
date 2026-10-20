#pragma once

#include "gcore/script/node.h"
#include "gcore/script/script_context.h"
#include "gcore/script/execution_defines.h"

namespace gtool
{
    void display_variable_node(struct imgui_serializer&, void*);
}

namespace gcore
{
    struct variable_node : node
    {
        using super = node;

        bool is_const() const override { return false; }
        void process(gserializer::serializer& serializer) override;
    protected:
        friend void gtool::display_variable_node(struct imgui_serializer&, void*);

        gtl::uuid m_variable_id;
        mutable pin_descriptor m_pin_descriptor;
        node_data_type::id_type m_type_id = 0;
    };

    struct get_variable_node : variable_node
    {
        using super = variable_node;
        GCORE_DECLARE_NODE_TYPE(get_variable_node);

        bool is_const() const override { return m_is_const; }

        void pre_create(script_descriptor& descriptor) override;
        void execute(node_context& context) const override;
        pin_descriptors get_pin_descriptors() const override;

    protected:
        bool m_is_const = false;
    };

    struct set_variable_node : variable_node
    {
        GCORE_DECLARE_NODE_TYPE(set_variable_node);
        using super = variable_node;
        using output_pin_descriptor = output_pin_descriptor<execution_pin_data, 0, 2>;
        void pre_create(script_descriptor& descriptor) override;
        void execute(node_context& context) const override;
        pin_descriptors get_pin_descriptors() const override;
    };
}
