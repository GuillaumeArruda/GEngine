#pragma once

#include <functional>
#include <vector>

namespace gtl
{
    using callback_id = std::size_t;
    template<class CallbackType>
    struct callbacks;

    template<class ... Args>
    struct callbacks<void (Args...)>
    {
        template<class CallbackLikeType>
        callback_id add_callback(CallbackLikeType&& callback)
        {
            m_callbacks.emplace_back(std::forward<CallbackLikeType>(callback), ++m_id_generator );
            return m_id_generator;
        }
        
        void remove_callback(callback_id id)
        {
            m_callbacks.erase(std::find(m_callbacks.begin(), m_callbacks.end(), id));
        }

        void operator()(Args... args)
        {
            for (auto& callback : m_callbacks)
                callback.m_function(args...);
        }

    private:
        struct callback
        {
            std::function<void (Args...)> m_function;
            std::size_t m_id = 0;
            bool operator==(callback_id const& rhs) const noexcept { return m_id == rhs; }
        };

        std::vector<callback> m_callbacks;
        callback_id m_id_generator;
    };
}