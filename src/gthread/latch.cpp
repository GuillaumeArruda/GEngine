#include "stdafx.h"

#include "gthread/latch.h"

namespace gthread
{
    void latch::count_down(int update)
    {
        std::unique_lock lock(m_lock);
        if(--m_count == 0)
            m_cond_var.notify_all();
    }

    void latch::count_down_and_wait()
    {
        ++m_waiting;
        {
            std::unique_lock lock(m_lock);
            if (--m_count == 0)
                m_cond_var.notify_all();
            else
                m_cond_var.wait(lock, [&] { return m_count <= 0; });
        }
        --m_waiting;
    }

    void latch::wait()
    {
        ++m_waiting;
        {
            std::unique_lock lock(m_lock);
            m_cond_var.wait(lock, [&] { return m_count <= 0; });
        }
        --m_waiting;
    }
}