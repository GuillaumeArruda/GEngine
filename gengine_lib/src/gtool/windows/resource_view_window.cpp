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
        if (ImGui::TreeNode("Create resource"))
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
            ImGui::TreePop();
        }
    }

    void resource_viewer_widget::update(gcore::resource_library& library)
    {
        if (ImGui::TreeNode("View Resource"))
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
                        write_json.write_to_file(library.get_filepath(uid).c_str());
                        s->unload();
                        s->load();
                    }
                    s.release();
                }
            }
            ImGui::TreePop();
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

            {
                grender::imgui_serializer serializer("Current Resources");
                serializer.process("Resources", library);
            }
            m_resource_viewer_widget.update(library);
            m_create_resource_widget.update(library);

            ImGui::End();
        }
    }
}
