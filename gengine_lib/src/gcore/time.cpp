#include "stdafx.h"
#include "gcore/time.h"


namespace gcore
{
    void time::new_frame()
    {
        if (m_is_paused)
            return;

        point const current_time = std::chrono::high_resolution_clock::now();
        m_delta_time = delta(m_start_frame, current_time);
        m_start_frame = current_time;
    }

    void time::pause()
    {
        m_is_paused = true;
    }

    void time::unpause()
    {
        m_is_paused = false;
        m_start_frame = std::chrono::high_resolution_clock::now();
    }

    float time::to_float(unit time)
    {
        return static_cast<float>(time.count()) / unit::period().den;
    }

    time::unit time::from_float(float time)
    {
        return unit(static_cast<unit::rep>(time * unit::period().den));
    }

    time::unit time::delta(point start, point end)
    {
        return  std::chrono::duration_cast<unit>(end - start);
    }
}

