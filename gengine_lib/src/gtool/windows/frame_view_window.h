#pragma once

#include <vector>
#include <chrono>

#include "gtool/window.h"

namespace gtool
{
    struct frame_view_window : window
    {
        void update(gcore::world& world, window_manager& manager) override;
        const char* get_name() const override { return "FPS"; }
        bool& get_should_display() override;
    private:
        std::vector<float> m_frame_times;
        std::chrono::time_point<std::chrono::high_resolution_clock> m_last_frame_begin = std::chrono::high_resolution_clock::now();
    };
}
