#include "stdafx.h"

#include "gcore/script/nodes/variable_nodes.h"
#include "grender/serializers/imgui_serializer.h"

#include <imgui/imgui.h>

namespace gcore
{
    void variable_node::pre_create(script_descriptor& descriptor)
    {
        super::pre_create(descriptor);
        if (auto it = std::find_if(descriptor.m_variables.begin(), descriptor.m_variables.end(), [&](auto& var) { return var.m_id == m_variable_id; });
            it != descriptor.m_variables.end())
        {
            m_type_id = it->m_data.get_type_id();
        }
    }

    void variable_node::process(gserializer::serializer& serializer)
    {
        super::process(serializer);
        if (grender::imgui_serializer* imgui = dynamic_cast<grender::imgui_serializer*>(&serializer))
        {
            if (auto* descriptor = serializer.get_in_context< std::reference_wrapper<script_descriptor>>())
            {
                std::string current_var_name;
                if (auto it = std::find_if(descriptor->get().m_variables.begin(), descriptor->get().m_variables.end(), [&](auto const& var) { return m_variable_id == var.m_id; });
                    it != descriptor->get().m_variables.end())
                {
                    current_var_name = std::format("{0} - {1}", it->m_name, it->m_id.to_string());
                }
                else
                {
                    m_variable_id = gtl::uuid();
                    m_type_id = 0;
                }

                if (ImGui::BeginCombo("Variable", current_var_name.c_str()))
                {
                    for (auto const& variable : descriptor->get().m_variables)
                    {
                        std::string const var_name = std::format("{0} - {1}", variable.m_name, variable.m_id.to_string());
                        bool is_selected = m_variable_id == variable.m_id;
                        if (ImGui::Selectable(var_name.c_str(), &is_selected))
                        {
                            m_variable_id = variable.m_id;
                            m_type_id = variable.m_data.get_type_id();
                        }

                        if (is_selected)
                            ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }
            }
        }
        else
        {
            serializer.process("variable_id", m_variable_id);
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