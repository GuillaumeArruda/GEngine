#pragma once

#include <chrono>

namespace gcore
{
    struct time
    {
        using point = std::chrono::time_point<std::chrono::high_resolution_clock>;
        using unit = std::chrono::microseconds;

        void new_frame();
        void pause();
        void unpause();

        unit get_delta_time() const { return m_delta_time; }
        point get_current_time() const { return m_start_frame; }

        static float to_float(unit time);
        static unit from_float(float time);
        static unit delta(point start, point end);

    private:
        point m_start_frame = std::chrono::high_resolution_clock::now();
        unit m_delta_time;
        bool m_is_paused = false;
    };


}
