#pragma once

#include <deque>
#include <optional>
#include <mutex>

#include "gthread/semaphore.h"

namespace gthread
{
    template<class T, std::uint64_t QueueDepth>
    struct concurrent_bonded_queue
    {
        template<class TLike>
        void enqueue(TLike&& obj)
        {
            m_spaces.acquire();
            {
                std::unique_lock lock(m_item_lock);
                m_elements.emplace_back(std::forward<TLike>(obj));
            }
            m_produced.release();
        }

        T dequeue()
        {
            m_produced.acquire();
            T tmp = pop();
            m_spaces.release();
            return std::move(tmp);
        }

        std::optional<T> try_dequeue()
        {
            if (!m_produced.try_acquire()) return {};
            T tmp = pop();
            m_spaces.release();
            return std::move(tmp);
        }

    private:
        T pop()
        {
            std::optional<T> tmp;
            {
                std::unique_lock lock(m_item_lock);
                tmp = std::move(m_elements.front());
                m_elements.pop_front();
            }
            return std::move(*tmp);
        }

        counting_semaphore<QueueDepth> m_produced{ 0 };
        counting_semaphore<QueueDepth> m_spaces{ QueueDepth };
        std::mutex m_item_lock;
        std::deque<T> m_elements;
    };
}
