#include "stdafx.h"

#include <imgui/imgui.h>
#include <optick/optick.h>

#include "gcore/console.h"

#include "gtool/windows/frame_view_window.h"

GCORE_REGISTER_CONSOLE_VALUE("fps.number_of_sample", int, fps_number_of_sample, 1000);
GCORE_REGISTER_CONSOLE_VALUE("fps.show", bool, fps_show, false);

namespace gtool
{
    void frame_view_window::update(gcore::world&, window_manager&)
    {
        OPTICK_EVENT();
        auto const now = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> const frame_time = now - m_last_frame_begin;
        m_last_frame_begin = now;

        if (m_frame_times.size() >= fps_number_of_sample && fps_number_of_sample > 0)
        {
            m_frame_times.erase(m_frame_times.begin(), m_frame_times.begin() + (m_frame_times.size() - fps_number_of_sample) + 1);
        }

        m_frame_times.push_back(frame_time.count() * std::chrono::microseconds::period::den);

        if (!fps_show)
            return;


        if (!ImGui::Begin(get_name(), &fps_show))
        {
            ImGui::End();
            return;
        }

        float average = 0.f;
        float min = std::numeric_limits<float>::max();
        float max = 0.f;
        for (float time : m_frame_times)
        {
            average += time;
            min = std::min(min, time);
            max = std::max(max, time);
        }
        if (m_frame_times.size() != 0)
            average /= m_frame_times.size();

        ImGui::Text("Average: %.0fus", average); ImGui::SameLine();
        ImGui::Text("Max: %.0fus", max); ImGui::SameLine();
        ImGui::Text("Min: %.0fus", min); ImGui::SameLine();
        ImGui::Text("Fps: %.2f", 1.f / (average / std::chrono::microseconds::period::den));
        ImGui::PlotHistogram("Frame time", [](void* data, int idx) { return *(static_cast<float*>(data) + idx); }, m_frame_times.data(), static_cast<int>(m_frame_times.size()), 0, nullptr, 0.f, average * 2.f, ImGui::GetContentRegionAvail());
        ImGui::End();
    }

    bool& frame_view_window::get_should_display()
    {
        return fps_show;
    }
}

