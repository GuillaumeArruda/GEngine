#include "stdafx.h"
#include "gtool/systems/create_entity_systems.h"

#include <imgui.h>
#include "imgui_impl/imgui_stdlib.h"

#include "gserializer/serializers/json_serializer.h"

#include "gcore/entity_registry.h"

#include "grender/serializers/imgui_serializer.h"

namespace gtool
{

    component_type_combo_box::component_type_combo_box()
    {
        m_component_type_names = gcore::component::factory().get_possible_type_names();
        if (m_component_type_names.size() > 0)
            m_current_component_type = m_component_type_names.front().c_str();
    }

    void component_type_combo_box::draw(const char* name)
    {
        if (ImGui::BeginCombo(name, m_current_component_type))
        {
            for (std::string const& string : m_component_type_names)
            {
                bool is_selected = m_current_component_type == string.c_str();
                if (ImGui::Selectable(string.c_str(), &is_selected))
                    m_current_component_type = string.c_str();

                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
    }

    void create_entity_system::update(gcore::entity_registry& registry)
    {
        if (!ImGui::Begin("Create entity tool", &m_display))
        {
            ImGui::End();
            return;
        }

        {
            grender::imgui_serializer serializer("Current Entities");
            serializer.process("Entities", registry);
        }

        selected_entity_widget.update(registry);
        create_entity_widget.update(registry);
        save_load_registry_widget.update(registry);
        

        ImGui::End();
    }

    void create_entity_widget::update(gcore::entity_registry& registry)
    {
        if (ImGui::TreeNode("New Entity"))
        {
            m_create_component_combo.draw("Create Component Type");

            {
                grender::imgui_serializer serializer("Current Components");
                serializer.process("Components", m_components, gcore::component::factory());
            }

            if (ImGui::Button("Create Component"))
            {
                if (auto comp = gcore::component::factory().create(m_create_component_combo.get_selected_component_type()))
                {
                    m_components.push_back(std::move(comp));
                }
            }
            ImGui::SameLine();

            if (ImGui::Button("Remove Component"))
            {
                m_components.erase(
                    std::remove_if(m_components.begin(), m_components.end(),
                        [&](auto& comp)
                {
                    return !comp || gcore::component::factory().get_type_name(*comp) == m_create_component_combo.get_selected_component_type();
                }), m_components.end());
            }
            ImGui::SameLine();

            if (ImGui::Button("Create entity with components"))
            {
                auto entity = registry.create_entity();
                registry.add_components(entity, m_components);
                m_components.clear();
            }
            ImGui::TreePop();
        }
    }
    void save_load_registry_widget::update(gcore::entity_registry& registry)
    {
        if (ImGui::TreeNode("Save/Load entities"))
        {
            ImGui::InputText("Filepath", &m_filepath);
            if (ImGui::Button("Save"))
            {
                gserializer::json_write_serializer json_write;
                json_write.process("entity_registry", registry);
                json_write.write_to_file(m_filepath.c_str());
            }
            ImGui::SameLine();
            if (ImGui::Button("Load"))
            {
                gserializer::json_read_serializer json_read(m_filepath.c_str());
                json_read.process("entity_registry", registry);
            }
            ImGui::TreePop();
        }
    }

    void selected_entity_widget::update(gcore::entity_registry& registry)
    {
        if (ImGui::TreeNode("Selected entity"))
        {
            ImGui::InputText("uuid", &m_uuid);
            
            gcore::entity const entity = gtl::uuid::from_string(m_uuid);
            if (registry.has_any_component(entity))
            {
                m_component_combo_box.draw("Component Type");

                grender::imgui_serializer serializer("Current Components");
                gtl::span<std::unique_ptr<gcore::component>> components = registry.see_components(entity);
                serializer.process("Components", components);

                if (ImGui::Button("Remove entity"))
                {
                    registry.remove_entity(entity);
                }
                ImGui::SameLine();
                if (ImGui::Button("Add Component"))
                {
                    if (auto comp = gcore::component::factory().create(m_component_combo_box.get_selected_component_type()))
                    {
                        registry.add_components(entity, { comp });
                    }
                }
                ImGui::SameLine();
                if (ImGui::Button("Remove Component"))
                {
                    registry.remove_component(entity, gcore::component::factory().get_type_index_from_name(m_component_combo_box.get_selected_component_type()));
                }
            }

            ImGui::TreePop();
        }
    }
}
