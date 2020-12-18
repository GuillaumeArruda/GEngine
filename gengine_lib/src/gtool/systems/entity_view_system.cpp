#include "stdafx.h"
#include "gtool/systems/entity_view_system.h"

#include <imgui.h>
#include "imgui_impl/imgui_stdlib.h"

#include "gserializer/serializers/json_serializer.h"

#include "gcore/entity_registry.h"

#include "grender/serializers/imgui_serializer.h"

namespace gtool
{
    void entity_view_system::update(gcore::entity_registry& registry)
    {
        if (!ImGui::Begin("Entity viewer", &m_display))
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
                serializer.process("Components", m_components, "Component", gcore::component::factory());
            }

            if (ImGui::Button("Create Component"))
            {
                if (auto comp = gcore::component::factory().create(m_create_component_combo.get_current_type()))
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
                    return !comp || gcore::component::factory().get_type_name(*comp) == m_create_component_combo.get_current_type();
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
                serializer.process("Components", components, "Component", gcore::component::factory());

                if (ImGui::Button("Remove entity"))
                {
                    registry.remove_entity(entity);
                }
                ImGui::SameLine();
                if (ImGui::Button("Add Component"))
                {
                    if (auto comp = gcore::component::factory().create(m_component_combo_box.get_current_type()))
                    {
                        registry.add_components(entity, { comp });
                    }
                }
                ImGui::SameLine();
                if (ImGui::Button("Remove Component"))
                {
                    registry.remove_component(entity, gcore::component::factory().get_type_index_from_name(m_component_combo_box.get_current_type()));
                }
            }

            ImGui::TreePop();
        }
    }
}
