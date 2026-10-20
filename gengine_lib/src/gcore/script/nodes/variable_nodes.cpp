#include "stdafx.h"

#include "gcore/script/nodes/variable_nodes.h"
#include "gtool/serializers/imgui_serializer.h"

#include <imgui/imgui.h>

namespace gcore
{
    void variable_node::process(gserializer::serializer& serializer)
    {
        super::process(serializer);
        serializer.process("variable_id", m_variable_id);
    }

    void get_variable_node::pre_create(script_descriptor& descriptor)
    {
        super::pre_create(descriptor);
        if (auto it = std::find_if(descriptor.m_variables.begin(), descriptor.m_variables.end(), [&](auto& var) { return var.m_id == m_variable_id; });
            it != descriptor.m_variables.end())
        {
            m_type_id = it->m_data.get_type_id();
            m_is_const = it->m_is_constant;
        }
    }

    void get_variable_node::execute(node_context& context) const
    {
        if (node_data const* var = context.get_variable(m_variable_id))
        {
            context.copy_data_to_output(*var, 0);
        }
    }

    node::pin_descriptors get_variable_node::get_pin_descriptors() const
    {
        m_pin_descriptor = pin_descriptor{ "value", 0, 1, m_type_id, pin_type::output };
        return pin_descriptors{ gtl::span<pin_descriptor const>(), gtl::span<pin_descriptor const>(m_pin_descriptor)};
    }

    void set_variable_node::pre_create(script_descriptor& descriptor)
    {
        super::pre_create(descriptor);
        if (auto it = std::find_if(descriptor.m_variables.begin(), descriptor.m_variables.end(), [&](auto& var) { return var.m_id == m_variable_id; });
            it != descriptor.m_variables.end())
        {
            if (it->m_is_constant)
            {
                m_variable_id = gtl::uuid();
                m_type_id = 0;
            }
            else
            {
                m_type_id = it->m_data.get_type_id();
            }
        }
    }

    void set_variable_node::execute(node_context& context) const
    {
        if (node_data* var = context.get_variable(m_variable_id))
        {
            if (node_data const* input_data = context.get_input_node_data(m_pin_descriptor.m_index))
            {
                (*var) = (*input_data);
            }
        }
    }

    node::pin_descriptors set_variable_node::get_pin_descriptors() const
    {
        m_pin_descriptor = pin_descriptor{ "value", 0, 1, m_type_id, pin_type::input };
        static pin_descriptor const output_desciptor = output_pin_descriptor::get("execute");
        return pin_descriptors{ m_pin_descriptor, output_desciptor };
    }
}