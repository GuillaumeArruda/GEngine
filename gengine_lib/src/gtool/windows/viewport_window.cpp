#include "stdafx.h"
#include "gtool/windows/viewport_window.h"

#include <imgui/imgui.h>

#include "gcore/world.h"
#include "gcore/systems/input_system.h"

#include "grender/systems/render_system.h"

#include "gtool/window_manager.h"

namespace gtool
{
    void viewport_window::update(gcore::world& world, window_manager& manager)
    {
        if (m_display)
        {
            if (!ImGui::Begin(get_name(), &m_display))
            {
                ImGui::End();
            }

            if (gcore::input_system * input = world.get_system_registry().get_system<gcore::input_system>())
            {
                if (m_display)
                {
                    input->m_window = manager.get_window();
                }
                else
                {
                    input->m_window = nullptr;
                }
            }

            if (grender::render_system* render_sys = world.get_system_registry().get_system<grender::render_system>())
            {
                ImVec2 const windowSize = ImGui::GetWindowSize();
                if (!render_sys->set_target_size(static_cast<std::size_t>(windowSize[0]), static_cast<std::size_t>(windowSize[1])))
                {
                    std::size_t const target_id = static_cast<std::size_t>(render_sys->get_target_id());
                    glm::ivec2 const size = render_sys->get_target_size();

                    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.f,0.f));
                    ImGui::Image(reinterpret_cast<void*>(target_id), ImVec2(static_cast<float>(size[0]), static_cast<float>(size[1])), ImVec2(0.f, 1.f), ImVec2(1.f, 0.f));
                    ImGui::PopStyleVar();
                }
            }

            ImGui::End();
        }
    }

}

