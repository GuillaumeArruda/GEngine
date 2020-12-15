#pragma once

#include <deque>
#include <mutex>
#include <optional>

namespace gthread
{
    template<class T>
    struct concurrent_queue
    {
        using value_type = T;
        void done()
        {
            {
                std::unique_lock lock(m_item_lock);
                m_done = true;
            }
            m_condVar.notify_all();
        }
        
        template<class TLike>
        bool try_enqueue(TLike&& obj)
        {
            {
                std::unique_lock lock(m_item_lock, std::try_to_lock);
                if (!lock) return false;
                m_elements.emplace_back(std::forward<TLike>(obj));
            }
            m_condVar.notify_one();
            return true;
        }
        
        template<class TLike>
        void enqueue(TLike&& obj)
        {
            {
                std::unique_lock lock(m_item_lock);
                m_elements.emplace_back(std::forward<TLike>(obj));
            }
            m_condVar.notify_one();
        }

        bool dequeue(T& outValue)
        {
            std::unique_lock lock(m_item_lock);
            while(m_elements.empty() && !m_done) m_condVar.wait(lock);
            if (m_elements.empty()) return false;
            outValue = std::move(m_elements.front());
            m_elements.pop_front();
            return true;
        }

        bool try_dequeue(T& outValue)
        {
            std::unique_lock lock(m_item_lock, std::try_to_lock);
            if (!lock || m_elements.empty()) return false;

            outValue = std::move(m_elements.front());
            m_elements.pop_front();
            return true;
        }

    private:
        std::mutex m_item_lock;
        std::condition_variable m_condVar;
        std::deque<T> m_elements;
        bool m_done = false;
    };
}