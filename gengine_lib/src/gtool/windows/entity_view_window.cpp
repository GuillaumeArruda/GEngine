#include "stdafx.h"
#include "gtool/windows/entity_view_window.h"

#include <imgui/imgui.h>
#include <imgui/imgui_stdlib.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <optick/optick.h>

#include "gserializer/serializers/json_serializer.h"

#include "gcore/components/extent_component.h"
#include "gcore/components/input_component.h"
#include "gcore/components/transform_component.h"
#include "gcore/components/name_component.h"
#include "gcore/serializers/dependency_gatherer_serializer.h"
#include "gcore/world.h"
#include "gcore/resource_library.h"

#include "grender/serializers/imgui_serializer.h"
#include "grender/components/camera_component.h"
#include "grender/systems/render_system.h"

#include "gserializer/serializers/binary_file_serializer.h"

namespace gtool
{
    void entity_view_window::update(gcore::world& world, window_manager&)
    {
        if (m_display)
        {
            OPTICK_EVENT();
            if (!ImGui::Begin(get_name(), &m_display))
            {
                ImGui::End();
                return;
            }

            m_selected_entity_widget.update(world);
            m_entity_browser_widget.update(world, m_selected_entity_widget);
            m_create_entity_widget.update(world);
            m_save_load_registry_widget.update(world);

            ImGui::End();
        }
    }

    void create_entity_widget::update(gcore::world& world)
    {
        if (ImGui::CollapsingHeader("New Entity"))
        {
            ImGui::PushID("New Entity");
            m_create_component_combo.draw("Create Component Type");

            {
                grender::imgui_serializer serializer("Current Components");
                serializer.set_in_context(std::ref(*world.get_resource_library()));
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
                auto entity = world.get_entity_registry().create_entity();
                world.get_entity_registry().add_components(entity, m_components);
                m_components.clear();
            }
            ImGui::PopID();
        }
    }

    void save_load_registry_widget::update(gcore::world& world)
    {
        if (ImGui::CollapsingHeader("Save/Load Entities"))
        {
            ImGui::PushID("Save/Load entities");
            ImGui::InputText("Filepath", &m_filepath);
            if (ImGui::Button("Save"))
            {
                gserializer::json_write_serializer json_write;
                json_write.process("entity_registry", world.get_entity_registry());
                json_write.write_to_file(m_filepath.c_str());
            }
            ImGui::SameLine();
            if (ImGui::Button("Load"))
            {
                gcore::dependency_gatherer_serializer gatherer;
                gatherer.process("", world.get_entity_registry());
                gcore::resource_library& lib = *world.get_resource_library();
                std::vector<gcore::resource_handle<gcore::resource>> resources; // Keep resource alive until end of serialization to avoid reloading everything
                for (auto const& uuid : gatherer.m_uuids)
                {
                    resources.push_back(lib.get_resource<gcore::resource>(uuid));
                }
                
                world.get_entity_registry().clear();
                
                gserializer::json_read_serializer json_read(m_filepath.c_str());
                gcore::layer_descriptor layer;
                json_read.set_in_context(std::ref(*world.get_resource_library()));
                json_read.process("entity_registry", layer);
                for (auto& [entity, components] : layer.m_entity_to_component)
                {
                    world.get_entity_registry().add_components(entity, components);
                }
            }
            ImGui::PopID();
        }
    }

    void selected_entity_widget::update(gcore::world& world)
    {
        if (grender::render_system* render = world.get_system_registry().get_system<grender::render_system>())
        {
            glm::ivec2 const size = render->get_target_size();
            float const aspect_ratio = size[0] / static_cast<float>(size[1]);
            auto camera_view = world.get_entity_registry().get_view<grender::camera_component, gcore::input_component, gcore::transform_component>();
            for (auto& [camera_entity, camera, input, camera_transform] : camera_view)
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

        if (ImGui::CollapsingHeader("Selected Entity"))
        {
            ImGui::PushID("Selected Entity");
            ImGui::InputText("UUID", &m_uuid);
            
            gcore::entity const entity = gtl::uuid::from_string(m_uuid);
            if (world.get_entity_registry().has_any_component(entity))
            {
                m_component_combo_box.draw("Component Type");

                grender::imgui_serializer serializer("Current Components");
                serializer.set_in_context(std::ref(*world.get_resource_library()));
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

            ImGui::PopID();
        }
    }

    void entity_browser_widget::update(gcore::world& world, selected_entity_widget& selected_widget)
    {
        if (ImGui::CollapsingHeader("Entity Browser"))
        {
            ImGui::PushID("Entity Browser");
            m_filter.Draw(); ImGui::SameLine();
            const char* display_name = [&] {
                switch (m_filter_by) {
                case column_id::name: return "Name";
                case column_id::uuid: return "UUID";
                default: return "";
                }
            }();
            if (ImGui::BeginCombo("Filter by", display_name))
            {
                bool is_selected = m_filter_by == column_id::name;
                if (ImGui::Selectable("Name", is_selected)) m_filter_by = column_id::name;
                if (is_selected) ImGui::SetItemDefaultFocus();

                is_selected = m_filter_by == column_id::uuid;
                if (ImGui::Selectable("UUID", is_selected)) m_filter_by = column_id::uuid;
                if (is_selected) ImGui::SetItemDefaultFocus();

                ImGui::EndCombo();
            }

            ImGuiTableFlags const table_flags = ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable | ImGuiTableFlags_Borders | ImGuiTableFlags_Sortable;
            if (ImGui::BeginTable("Entity", 2, table_flags))
            {
                ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_DefaultSort, -1.f, column_id::name);
                ImGui::TableSetupColumn("UUID", 2, -1.f, column_id::uuid);
                ImGui::TableHeadersRow();

                struct entity_info
                {
                    gtl::uuid m_uuid;
                    std::string* m_name;
                };

                std::vector<entity_info> infos;
                auto view = world.get_entity_registry().get_view<gcore::optional_comp<gcore::name_component>>();
                std::string empty_string;
                for (auto& [entity, name_comp] : view)
                {
                    infos.push_back(entity_info{ entity, name_comp ? &name_comp->m_name : &empty_string });
                }

                if (ImGuiTableSortSpecs* sort_spec = ImGui::TableGetSortSpecs())
                {
                        std::sort(infos.begin(), infos.end(),
                            [&](entity_info const& lhs, entity_info const& rhs) {
                            switch (sort_spec->Specs->ColumnIndex)
                            {
                            case column_id::uuid:
                                return sort_spec->Specs->SortDirection == ImGuiSortDirection_Ascending ?
                                    lhs.m_uuid < rhs.m_uuid :
                                    lhs.m_uuid >= rhs.m_uuid;
                            default:
                            case column_id::name:
                                return sort_spec->Specs->SortDirection == ImGuiSortDirection_Ascending ?
                                    *lhs.m_name < *rhs.m_name :
                                    *lhs.m_name >= *rhs.m_name;
                            }
                        });
                }

                ImGuiListClipper clipper;
                clipper.Begin(static_cast<int>(infos.size()));

                while (clipper.Step())
                {
                    for (int row_n = clipper.DisplayStart; row_n < clipper.DisplayEnd; row_n++)
                    {
                        if ((m_filter_by == column_id::name && m_filter.PassFilter(infos[row_n].m_name->c_str()))
                            || (m_filter_by == column_id::uuid && m_filter.PassFilter(infos[row_n].m_uuid.to_string().c_str())))
                        {
                            ImGui::TableNextColumn();
                            if (ImGui::Selectable(infos[row_n].m_name->c_str(), false, ImGuiSelectableFlags_SpanAllColumns))
                                selected_widget.m_uuid = infos[row_n].m_uuid.to_string();

                            ImGui::TableNextColumn(); ImGui::Text(infos[row_n].m_uuid.to_string().c_str());
                        }
                    }
                }

                ImGui::EndTable();
            }
            ImGui::PopID();
        }
    }
}
