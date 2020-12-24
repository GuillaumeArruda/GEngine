#pragma once
#include <memory>
#include <unordered_map>
#include <typeindex>

#include "gtl/cast.h"

#include "gcore/system.h"

namespace gcore
{
    struct world;

    using system_id = std::type_index;
    struct system_registry
    {
        system_registry() = default;
        ~system_registry();

        system_registry(system_registry&&) noexcept;
        system_registry& operator=(system_registry&&) noexcept;

        system_registry(system_registry const&) = delete;
        system_registry& operator=(system_registry const&) = delete;

        void add_system(std::unique_ptr<system> system);

        template<class SystemType>
        SystemType* get_system()
        {
            auto it = m_system_map.find(system_id(typeid(SystemType)));
            if (it != m_system_map.end())
                return gtl::cast<SystemType*>(it->second.get());
            return nullptr;
        }

        template<class SystemType>
        SystemType const* get_system() const
        {
            auto it = m_system_map.find(system_id(typeid(SystemType)));
            if (it != m_system_map.end())
                return gtl::cast<SystemType const *>(it->second.get());
            return nullptr;
        }

        void connect_systems_to_world(gcore::world& world);

    private:
        std::unordered_map<system_id, std::unique_ptr<system>> m_system_map;
    };
}