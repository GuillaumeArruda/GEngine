#pragma once

#include <mutex>

namespace gthread
{
    template<class Data>
    struct locked
    {
        struct wrapper
        {
            Data* operator->() { return &m_data; }
            Data const* operator->() const { return &m_data; }
            Data& m_data;
            std::unique_lock<std::mutex> m_lock;
        };

        wrapper get() { return wrapper{ m_data, std::unique_lock(m_mutex) }; }
        wrapper const get() const { return wrapper{ m_data, std::unique_lock(m_mutex) }; }

    protected:
        Data m_data;
        mutable std::mutex m_mutex;
    };
}