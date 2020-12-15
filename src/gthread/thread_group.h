#pragma once

#include <thread>
#include <vector>

namespace gthread
{
    struct thread_group
    {
        template<class T, class ... Args>
        thread_group(unsigned int number_of_thread, T&& callable, Args&& ... args)
        {
            m_threads.reserve(number_of_thread);
            for (unsigned int i = 0; i < number_of_thread; ++i)
            {
                m_threads.emplace_back(std::forward<T>(callable), i, std::forward<Args>(args)...);
            }
        }
        thread_group(thread_group const&) = delete;
        thread_group(thread_group&&) noexcept = default;

        thread_group& operator=(thread_group const&) = delete;
        thread_group& operator=(thread_group&&) noexcept = default;

        ~thread_group();

        std::size_t size() const noexcept { return m_threads.size(); }
        bool contains(std::thread::id thread_id) const noexcept { return std::find_if(m_threads.begin(), m_threads.end(), [&](std::thread const& thread) { return thread.get_id() == thread_id; }) != m_threads.end(); }
    private:
        std::vector<std::thread> m_threads;
    };
}