#include "stdafx.h"
#include "gtool/systems/resource_view_system.h"

#include <imgui.h>
#include "imgui_impl/imgui_stdlib.h"

#include "gtl/uuid.h"

#include "gserializer/serializers/json_serializer.h"

#include "gcore/resource_library.h"

#include "grender/serializers/imgui_serializer.h"

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
                    std::unique_ptr<gcore::resource> s(resource);
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

    void resouce_view_system::update(gcore::resource_library& library)
    {
        if (!ImGui::Begin("Resource viewer"))
        {
            ImGui::End();
            return;
        }

        {
            grender::imgui_serializer serializer("Current Resources");
            serializer.process("Resources", library);
        }
        m_resource_viewer_widget.update(library);
        m_create_resource_widget.update(library);

        ImGui::End();
    }

}
