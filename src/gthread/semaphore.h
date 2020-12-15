#pragma once

#include <mutex>
#include <atomic>

namespace gthread
{
    // Replacement for c++20 counting semaphore
    template<std::ptrdiff_t MaxCount>
    struct counting_semaphore
    {
        explicit counting_semaphore(std::ptrdiff_t desired) : m_count(desired) {};
        counting_semaphore(counting_semaphore const&) = delete;
        counting_semaphore(counting_semaphore&&) = delete;
        counting_semaphore& operator=(counting_semaphore const&) = delete;
        counting_semaphore& operator=(counting_semaphore&&) = delete;

        void acquire()
        {
            std::unique_lock lock(m_lock);    
            m_condVar.wait(lock, [this]{ return m_count > 0; });
            --m_count;
        }

        bool try_acquire()
        {
            std::unique_lock lock(m_lock);
            if (m_count == 0) return false;
            --m_count;
            return true;
        }

        void release()
        {
            {
                std::unique_lock lock(m_lock);
                m_count++;
            }
            m_condVar.notify_one();
        }

    private:
        std::mutex m_lock;
        std::condition_variable m_condVar;
        std::ptrdiff_t m_count = 0;
    };
}