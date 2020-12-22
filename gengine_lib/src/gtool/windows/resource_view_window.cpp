#include "stdafx.h"
#include "gtool/windows/resource_view_window.h"

#include <imgui/imgui.h>
#include "imgui/imgui_stdlib.h"

#include "gtl/uuid.h"

#include "gserializer/serializers/json_serializer.h"

#include "gcore/world.h"

#include "grender/serializers/imgui_serializer.h"
#include "resource_view_window.h"

namespace gtool
{
    void create_resource_widget::update(gcore::resource_library& library)
    {
        if (ImGui::CollapsingHeader("Create resource"))
        {
            m_resource_type.draw("Resource Type");
            ImGui::InputText("folder", &m_folder_path);
            if (ImGui::Button("Create resource"))
            {
                if (auto resource = gcore::resource::factory().create(m_resource_type.get_current_type()))
                {
                    gserializer::json_write_serializer write_json;
                    write_json.process("resource", resource, gcore::resource::factory());
                    write_json.write_to_file((m_folder_path + resource->get_uuid().to_string() + ".json").c_str());
                    library.scan_directory(m_folder_path.c_str());
                }
            }
        }
    }

    void resource_viewer_widget::update(gcore::resource_library& library)
    {
        if (ImGui::CollapsingHeader("View Resource"))
        {
            ImGui::InputText("uuid", &m_uuid);
            gtl::uuid const uid = gtl::uuid::from_string(m_uuid);
            if (uid.is_valid())
            {
                if (auto resource = library.get_resource<gcore::resource>(uid))
                {
                    std::unique_ptr<gcore::resource> s(resource.get());
                    grender::imgui_serializer serializer("Resource");
                    serializer.process("Resource", s, gcore::resource::factory());
                    if (ImGui::Button("Save"))
                    {
                        gserializer::json_write_serializer write_json;
                        write_json.process("resource", s, gcore::resource::factory());
                        write_json.write_to_file(library.get_filepath(uid).string().c_str());
                    }
                    s.release();
                }
            }
        }
    }

    void resource_view_window::update(gcore::world& world, window_manager&)
    {
        if (m_display)
        {
            if (!ImGui::Begin(get_name(), &m_display))
            {
                ImGui::End();
                return;
            }
            gcore::resource_library& library = *world.get_resource_library();

            m_resource_viewer_widget.update(library);
            m_create_resource_widget.update(library);
            m_resource_browser.update(library, m_resource_viewer_widget);

            ImGui::End();
        }
    }

    void resource_browser_widget::update(gcore::resource_library& library, resource_viewer_widget& viewer_widget)
    {
        if (ImGui::CollapsingHeader("Resource browser"))
        {
            if (m_resources_info.size() == 0)
            {
                auto& uuid_to_files = library.get_uuid_to_resource_files();
                for (auto& uuid_to_file : uuid_to_files)
                {
                    auto& res_info = m_resources_info.emplace_back();
                    res_info.m_uuid = uuid_to_file.first;

                    gserializer::json_read_serializer json_read(uuid_to_file.second.string().c_str());
                    json_read.open_scope("resource");
                    json_read.process("type_name", res_info.m_resource_type);
                    json_read.open_scope("data");
                    json_read.process("name", res_info.m_name);
                    json_read.close_scope("data");
                    json_read.close_scope("resource");
                }
            }

            static ImGuiTextFilter filter;
            filter.Draw(); ImGui::SameLine();
            const char* display_name = [&] {
                switch (m_filter_by) {
                case column_id::name: return "Name";
                case column_id::type: return "Type";
                case column_id::uuid: return "UUID";
                default: return "";
                }
            }();
            if (ImGui::BeginCombo("Filter by", display_name))
            {
                bool is_selected = m_filter_by == column_id::name;
                if (ImGui::Selectable("Name", is_selected)) m_filter_by = column_id::name;
                if (is_selected) ImGui::SetItemDefaultFocus();
                
                is_selected = m_filter_by == column_id::type;
                if (ImGui::Selectable("Type", is_selected)) m_filter_by = column_id::type;
                if (is_selected) ImGui::SetItemDefaultFocus();

                is_selected = m_filter_by == column_id::uuid;
                if (ImGui::Selectable("UUID", is_selected)) m_filter_by = column_id::uuid;
                if (is_selected) ImGui::SetItemDefaultFocus();

                ImGui::EndCombo();

            }
            ImGuiTableFlags const table_flags = ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable | ImGuiTableFlags_Borders | ImGuiTableFlags_Sortable;
            if (ImGui::BeginTable("Resources", 3, table_flags))
            {

                ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_DefaultSort, -1.f, column_id::name);
                ImGui::TableSetupColumn("Type", 1, -1.f, column_id::type);
                ImGui::TableSetupColumn("UUID", 2, -1.f, column_id::uuid);
                ImGui::TableHeadersRow();

                if (ImGuiTableSortSpecs* sort_spec = ImGui::TableGetSortSpecs())
                {
                    if (sort_spec->SpecsDirty)
                    {
                        std::sort(m_resources_info.begin(), m_resources_info.end(),
                            [&](resource_info const& lhs, resource_info const& rhs) {
                            switch (sort_spec->Specs->ColumnIndex)
                            {
                            case column_id::type: 
                                return sort_spec->Specs->SortDirection == ImGuiSortDirection_Ascending ?
                                    lhs.m_resource_type < rhs.m_resource_type :
                                    lhs.m_resource_type >= rhs.m_resource_type;
                            case column_id::uuid: 
                                return sort_spec->Specs->SortDirection == ImGuiSortDirection_Ascending ?
                                    lhs.m_uuid < rhs.m_uuid :
                                    lhs.m_uuid >= rhs.m_uuid;
                            default:
                            case column_id::name:
                                return sort_spec->Specs->SortDirection == ImGuiSortDirection_Ascending ?
                                    lhs.m_name < rhs.m_name :
                                    lhs.m_name >= rhs.m_name;
                            }
                        });
                        sort_spec->SpecsDirty = false;
                    }
                }
                ImGuiListClipper clipper;
                clipper.Begin(static_cast<int>(m_resources_info.size()));
                
                while (clipper.Step())
                {
                    for (int row_n = clipper.DisplayStart; row_n < clipper.DisplayEnd; row_n++)
                    {
                        if ((m_filter_by == column_id::name && filter.PassFilter(m_resources_info[row_n].m_name.c_str()))
                        ||  (m_filter_by == column_id::type && filter.PassFilter(m_resources_info[row_n].m_resource_type.c_str()))
                        ||  (m_filter_by == column_id::uuid && filter.PassFilter(m_resources_info[row_n].m_uuid.to_string().c_str())))
                        {
                            ImGui::TableNextColumn(); 
                            if (ImGui::Selectable(m_resources_info[row_n].m_name.c_str(), false, ImGuiSelectableFlags_SpanAllColumns))
                                ImGui::OpenPopup(m_resources_info[row_n].m_name.c_str());

                            if (ImGui::BeginPopup(m_resources_info[row_n].m_name.c_str()))
                            {
                                if (ImGui::Selectable("Open"))
                                {
                                    viewer_widget.m_uuid = m_resources_info[row_n].m_uuid.to_string().c_str();
                                    ImGui::CloseCurrentPopup();
                                }

                                if (ImGui::Selectable("Copy UUID"))
                                {
                                    ImGui::SetClipboardText(m_resources_info[row_n].m_uuid.to_string().c_str());
                                    ImGui::CloseCurrentPopup();
                                }
                                ImGui::EndPopup();
                            }

                            ImGui::TableNextColumn(); ImGui::Text(m_resources_info[row_n].m_resource_type.c_str());
                            ImGui::TableNextColumn(); ImGui::Text(m_resources_info[row_n].m_uuid.to_string().c_str());
                        }
                    }
                }
                ImGui::EndTable();
            }
        }
    }
}
