#include "stdafx.h"

#include "gtool/common_widgets.h"

#include <imgui.h>
#include "imgui_impl/imgui_stdlib.h"

namespace gtool
{
    namespace details
    {
        void factory_combo_box_base::draw(const char* name)
        {
            if (ImGui::BeginCombo(name, m_current_type))
            {
                for (std::string const& string : m_type_names)
                {
                    bool is_selected = m_current_type == string.c_str();
                    if (ImGui::Selectable(string.c_str(), &is_selected))
                        m_current_type = string.c_str();

                    if (is_selected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
        }
    }

}


