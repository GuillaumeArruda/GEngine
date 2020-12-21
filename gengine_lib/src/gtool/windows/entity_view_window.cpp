#include "stdafx.h"
#include "gtool/windows/entity_view_window.h"

#include <imgui/imgui.h>
#include <imgui/imgui_stdlib.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "gserializer/serializers/json_serializer.h"

#include "gcore/components/extent_component.h"
#include "gcore/components/input_component.h"
#include "gcore/components/transform_component.h"
#include "gcore/world.h"

#include "grender/serializers/imgui_serializer.h"
#include "grender/components/camera_component.h"
#include "grender/systems/render_system.h"

namespace gtool
{
    void entity_view_window::update(gcore::world& world, window_manager&)
    {
        if (m_display)
        {
            if (!ImGui::Begin(get_name(), &m_display))
            {
                ImGui::End();
                return;
            }

            gcore::entity_registry& registry = world.get_entity_registry();

            {
                grender::imgui_serializer serializer("Current Entities");
                serializer.process("Entities", registry);
            }

            selected_entity_widget.update(world);
            create_entity_widget.update(registry);
            save_load_registry_widget.update(registry);

            ImGui::End();
        }
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

    void selected_entity_widget::update(gcore::world& world)
    {
        if (grender::render_system* render = world.get_system_registry().get_system<grender::render_system>())
        {
            glm::ivec2 const size = render->get_target_size();
            float const aspect_ratio = size[0] / static_cast<float>(size[1]);
            auto camera_view = world.get_entity_registry().get_view<grender::camera_component, gcore::input_component, gcore::transform_component>();
            for (auto& [entity, camera, input, camera_transform] : camera_view)
            {
                if (input->m_keybord_state[gtl::to_underlying(gcore::keyboard_key::left_control)] == gcore::key_state::pressed
                    && input->m_mouse_key_state[gtl::to_underlying(gcore::mouse_key::button_1)] == gcore::key_state::pressed)
                {
                    using gmath::position; using gmath::direction; using gmath::ray;
                    using gcore::model_space; using gcore::world_space; using gcore::camera_space; using gcore::projection_space;

                    glm::mat4 const from_projection_space = glm::inverse(glm::perspective(static_cast<float>(gmath::radian(camera->m_fov)), aspect_ratio, camera->m_near_z, camera->m_far_z));
                    glm::vec2 const device_coordinates = (input->m_mouse_positions[0] * 2.f) - glm::vec2(1.f);
                    glm::vec4 const camera_direction = from_projection_space * glm::vec4(device_coordinates[0], -device_coordinates[1], -1.f, 1.f);
                    glm::vec4 const world_direction = camera_transform->m_transform * glm::vec4(camera_direction[0], camera_direction[1], -1.f, 0.f);
                    ray<world_space> const world_space_ray = ray<world_space>(gmath::position<world_space>(camera_transform->m_transform[3]), gmath::direction<world_space>(glm::vec3(world_direction)));

                    float best_t = std::numeric_limits<float>::max();
                    gcore::entity best_entity;
                    auto extent_view = world.get_entity_registry().get_view<gcore::transform_component, gcore::extent_component>();
                    for (auto& [entity, transform, extent] : extent_view)
                    {
                        gmath::axis_aligned_box<world_space> const projection_extent = gmath::transform<world_space, model_space>(transform->m_transform) * extent->m_extent;
                        auto result = projection_extent.intersect(world_space_ray);
                        if (result && result->t0 < best_t)
                        {
                            best_t = result->t0;
                            best_entity = entity;
                        }
                    }

                    m_uuid = best_entity.to_string();
                }
            }
        }

        if (ImGui::TreeNode("Selected entity"))
        {
            ImGui::InputText("uuid", &m_uuid);
            
            gcore::entity const entity = gtl::uuid::from_string(m_uuid);
            if (world.get_entity_registry().has_any_component(entity))
            {
                m_component_combo_box.draw("Component Type");

                grender::imgui_serializer serializer("Current Components");
                gtl::span<std::unique_ptr<gcore::component>> components = world.get_entity_registry().see_components(entity);
                serializer.process("Components", components, "Component", gcore::component::factory());

                if (ImGui::Button("Remove entity"))
                {
                    world.get_entity_registry().remove_entity(entity);
                }
                ImGui::SameLine();
                if (ImGui::Button("Add Component"))
                {
                    if (auto comp = gcore::component::factory().create(m_component_combo_box.get_current_type()))
                    {
                        world.get_entity_registry().add_components(entity, { comp });
                    }
                }
                ImGui::SameLine();
                if (ImGui::Button("Remove Component"))
                {
                    world.get_entity_registry().remove_component(entity, gcore::component::factory().get_type_index_from_name(m_component_combo_box.get_current_type()));
                }
            }

            ImGui::TreePop();
        }
    }
}
