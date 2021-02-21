#pragma once

#include <unordered_map>
#include <typeindex>
#include <any>

namespace gtl
{
    struct any_map
    {
        template<class ContextElement>
        void set(ContextElement&& element)
        {
            m_map[std::type_index(typeid(ContextElement))] = std::forward<ContextElement>(element);
        }

        template<class ContextElement>
        ContextElement* get()
        {
            auto it = m_map.find(std::type_index(typeid(ContextElement)));
            if (it != m_map.end())
            {
                if (it->second.type() == typeid(ContextElement))
                    return &std::any_cast<ContextElement&>(it->second);
            }
            return nullptr;
        }

        template<class ContextElement>
        ContextElement const* get() const
        {
            auto it = m_map.find(std::type_index(typeid(ContextElement)));
            if (it != m_map.end())
            {
                if (it->second.type() == typeid(ContextElement))
                    return &std::any_cast<ContextElement&>(it->second);
            }
            return nullptr;
        }

        template<class ContextElement>
        bool has() const
        {
            return m_map.find(std::type_index(type_id(ContextElement))) != m_map.end();
        }

    private:
        std::unordered_map<std::type_index, std::any> m_map;
    };
}