#pragma once
#include <memory>
#include <unordered_map>

#include "gtl/cast.h"

#include "gcore/system.h"

namespace gcore
{
    struct system_registry
    {
        system_registry() = default;
        ~system_registry();

        system_registry(system_registry&&);
        system_registry& operator=(system_registry&&) noexcept;

        system_registry(system_registry const&) = delete;
        system_registry& operator=(system_registry const&) = delete;

        void add_system(std::unique_ptr<system> system);

        template<class SystemType>
        system* get_system()
        {
            auto it = m_system_map.find(typeid(SystemType).hash_code());
            if (it != m_system_map.end())
                return gtl::cast<SystemType*>(it->second.get());
            return nullptr;
        }

        template<class SystemType>
        system const* get_system() const
        {
            auto it = m_system_map.find(typeid(SystemType).hash_code());
            if (it != m_system_map.end())
                return gtl::cast<SystemType const *>(it->second.get());
            return nullptr;
        }
    private:
        std::unordered_map<std::size_t, std::unique_ptr<system>> m_system_map;
    };
}