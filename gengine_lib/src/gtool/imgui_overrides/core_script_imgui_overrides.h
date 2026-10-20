#pragma once

#include <imgui/imgui.h>

#include "gcore/script/node_data.h"
#include "gcore/script/nodes/variable_nodes.h"
#include "gtool/serializers/imgui_serializer.h"
#include "gcore/script/script_runner.h"

namespace gtool
{
    struct imgui_serializer;
    void display_node_data(imgui_serializer& serializer, void* address);
    void display_variable_node(imgui_serializer& serializer, void* address);
    void display_node_descriptor(imgui_serializer& serializer, void* address);

    template<class ScriptType>
    void display_script_runner(imgui_serializer& serializer, void* address)
    {
        using script_runner = gcore::script_runner<ScriptType>;
        script_runner& runner = *static_cast<script_runner*>(address);
        serializer.process("script", runner.m_script);
        if (runner.m_script.get_uuid() == gtl::uuid())
        {
            runner.m_inputs.clear();
            return;
        }
        
        serializer.process("inputs", runner.m_inputs, "input");
        if (runner.m_script.is_loaded())
        {
            gcore::script_descriptor const& descriptor = runner.m_script->get_descriptor();
            std::vector<gcore::script_descriptor::variable> variables = descriptor.m_variables;
            std::erase_if(variables, [&](auto& variable) {
                return !variable.m_is_input || std::any_of(runner.m_inputs.begin(), runner.m_inputs.end(), [&](auto& input) {
                    return input.m_variable_id == variable.m_id;
                });
            });
            
            if (variables.size() > 0)
            {
                static std::size_t selected_variable_index = 0;
                if (selected_variable_index >= variables.size())
                {
                    selected_variable_index = 0;
                }

                if (ImGui::BeginCombo("Variable", (variables[selected_variable_index].m_name + " - " + variables[selected_variable_index].m_id.to_string()).c_str()))
                {
                    for (std::size_t index = 0; index < variables.size(); ++index)
                    {
                        bool is_selected = selected_variable_index == index;
                        if (ImGui::Selectable((variables[index].m_name + " - " + variables[index].m_id.to_string()).c_str(), &is_selected))
                            selected_variable_index = index;

                        if (is_selected)
                            ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }
                ImGui::SameLine();
                
                if (ImGui::Button("Create Input"))
                {
                    runner.m_inputs.emplace_back(variables[selected_variable_index].m_id, variables[selected_variable_index].m_data);
                }
            }
        }
    }
}