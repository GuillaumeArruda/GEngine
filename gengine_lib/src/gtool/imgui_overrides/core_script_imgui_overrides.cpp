#include <stdafx.h>

#include <imgui/imgui.h>

#include "gtool/imgui_overrides/core_script_imgui_overrides.h"
#include "gtool/serializers/imgui_serializer.h"

#include "gcore/script/node_data_type.h"

namespace gtool
{
    void display_node_data(imgui_serializer& serializer, void* address)
    {
        gcore::node_data& data = *static_cast<gcore::node_data*>(address);

        auto& registry = gcore::node_data_type_registry::get();
        std::string type_name;
        if (data.m_type_id != gcore::node_data_type::invalid_id)
        {
            type_name = registry.get_node_data_type(data.m_type_id).get_name();
        }

        if (!serializer.should_display())
        {
            return;
        }

        if (ImGui::BeginCombo("Type", type_name.c_str()))
        {
            for (auto const& node_data_type : registry.get_node_data_types())
            {
                bool is_selected = type_name == node_data_type.get_name();
                if (ImGui::Selectable(node_data_type.get_name().c_str(), &is_selected))
                    type_name = node_data_type.get_name();

                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        gcore::node_data_type const* new_node_type = registry.get_by_name(type_name);
        if (!new_node_type)
        {
            data.clear();
            return;
        }
        data.set_type(new_node_type, data.get_number_of_elements());

        if (data.m_is_inline)
        {
            new_node_type->m_vtable.m_process(serializer, data.m_inline_info.m_buffer, 1);
        }
        else
        {
            new_node_type->m_vtable.m_process(serializer, data.m_pointer_info.m_pointer_to_data, data.m_pointer_info.m_number_of_elements);
        }

        size_t number_of_elements = data.get_number_of_elements();
        if (ImGui::Button("Create Element"))
        {
            gcore::node_data new_data;
            new_data.m_type_id = data.m_type_id;
            void* array = new_data.create_array_internal(number_of_elements + 1);
            new_node_type->m_vtable.m_move_over_elements(data.m_is_inline ? data.m_inline_info.m_buffer : data.m_pointer_info.m_pointer_to_data, array, number_of_elements);
            data = std::move(new_data);
        }

        ImGui::SameLine();

        if (ImGui::Button("Remove Element") && number_of_elements != 0)
        {
            gcore::node_data new_data;
            new_data.m_type_id = data.m_type_id;
            void* array = new_data.create_array_internal(number_of_elements - 1);
            new_node_type->m_vtable.m_move_over_elements(data.m_is_inline ? data.m_inline_info.m_buffer : data.m_pointer_info.m_pointer_to_data, array, number_of_elements - 1);
            data = std::move(new_data);
        }
    }

    void display_variable_node(imgui_serializer& serializer, void* address)
    {
        gcore::variable_node& data = *static_cast<gcore::variable_node*>(address);
        if (auto* descriptor = serializer.get_in_context<std::reference_wrapper<gcore::script_descriptor>>())
        {
            std::string current_var_name;
            if (auto it = std::find_if(descriptor->get().m_variables.begin(), descriptor->get().m_variables.end(), [&](auto const& var) { return data.m_variable_id == var.m_id; });
                it != descriptor->get().m_variables.end())
            {
                current_var_name = std::format("{0} - {1}", it->m_name, it->m_id.to_string());
            }
            else
            {
                data.m_variable_id = gtl::uuid();
                data.m_type_id = 0;
            }

            if (ImGui::BeginCombo("Variable", current_var_name.c_str()))
            {
                for (auto const& variable : descriptor->get().m_variables)
                {
                    std::string const var_name = std::format("{0} - {1}", variable.m_name, variable.m_id.to_string());
                    bool is_selected = data.m_variable_id == variable.m_id;
                    if (ImGui::Selectable(var_name.c_str(), &is_selected))
                    {
                        data.m_variable_id = variable.m_id;
                        data.m_type_id = variable.m_data.get_type_id();
                    }

                    if (is_selected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
        }
    }
}