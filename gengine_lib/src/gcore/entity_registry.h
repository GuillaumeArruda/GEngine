#pragma once

#include <unordered_map>
#include <vector>
#include <memory>
#include <tuple>
#include <shared_mutex>
#include <typeindex>

#include "gtl/span.h"
#include "gtl/cast.h"
#include "gtl/uuid.h"

#include "gcore/component.h"

namespace gserializer
{
    struct serializer;
}


namespace gcore
{
    using entity = gtl::uuid;
    using component_id = std::type_index;
    using view_id = std::type_index;

    struct entity_registry
    {
        [[nodiscard]] entity create_entity();
        void remove_entity(entity entity);
        
        void add_components(entity entity, gtl::span<std::unique_ptr<component>> components);
        template<class ComponentType>
        void remove_component(entity entity) { remove_component(entity, component_id(typeid(ComponentType))); }

        template<class ... ComponentType>
        [[nodiscard]] auto get_components(entity entity) const
        {
            if constexpr (sizeof...(ComponentType) == 1)
            {
                return gtl::cast<ComponentType*...>(get_component(entity, component_id(typeid(ComponentType)...)));
            }
            else
            {
                return std::tuple<ComponentType*...>{ gtl::cast<ComponentType*>(get_component(entity, component_id(typeid(ComponentType))))... };
            }
        }

        template<class ... ComponentType>
        [[nodiscard]] auto get_components(entity entity)
        {
            if constexpr (sizeof...(ComponentType) == 1)
            {
                return gtl::cast<ComponentType*...>(get_component(entity, component_id(typeid(ComponentType)...)));
            }
            else
            {
                return std::tuple<ComponentType*...>{ gtl::cast<ComponentType*>(get_component(entity, component_id(typeid(ComponentType))))... };
            }
        }


        template<class ... ComponentType>
        [[nodiscard]] bool has_components(entity) const { 
            static component_id component_types[] = { std::type_index(typeid(ComponentType))... };
            return has_component(entity, component_types); 
        }

        template<class ...>
        struct view;

        template<class ... ComponentType>
        [[nodiscard]] view<ComponentType...> get_view();

        void process(gserializer::serializer& serializer);

        void remove_component(entity entity, component_id component_type);
        [[nodiscard]] component* get_component(entity entity, component_id component_type) const;

        gtl::span<std::unique_ptr<component>> see_components(entity entity);
        [[nodiscard]] bool has_any_component(entity entity) const;

        void rebuild_component_type_map();
    private:
        bool has_components(entity entity, gtl::span<component_id const> component_types) const;

        struct group_base
        {
            virtual ~group_base() {}
            virtual void on_component_added(entity_registry const& registry, entity entity) = 0;
            virtual void on_component_removed(entity_registry const& registry, entity entity) = 0;
            virtual gtl::span<component_id const> get_component_types() const noexcept = 0;
        };

        template<class ... ComponentTypes>
        struct group_impl;
        
        struct entity_component_list
        {
            std::vector<entity> m_entities;
            std::vector<component*> m_components;
        };
        
        std::unordered_map<entity, std::vector<std::unique_ptr<component>>> m_entity_to_component;
        std::unordered_map<component_id, entity_component_list> m_component_type_map;
        std::unordered_map<view_id, std::unique_ptr<group_base>> m_group_map;
        std::shared_mutex m_lock;
    };

    template<class ... ComponentTypes>
    struct entity_registry::group_impl : entity_registry::group_base
    {

    private:
        friend view;
        friend entity_registry;
        
        void on_component_added(entity_registry const& registry, entity new_entity) override
        {
            auto const it = std::lower_bound(m_entities_components.begin(), m_entities_components.end(), new_entity,
                [](std::tuple<entity, ComponentTypes*...> const& tuple, entity const& to_find) {
                return to_find < std::get<entity>(tuple);
            });

            if (it == m_entities_components.end() || std::get<entity>(*it) != new_entity)
            {
                if (registry.has_components(new_entity, m_component_ids))
                {
                    if constexpr (sizeof...(ComponentTypes) == 1)
                    {
                        m_entities_components.insert(it, std::tuple(new_entity, registry.get_components<ComponentTypes...>(new_entity)));
                    }
                    else
                    {
                        m_entities_components.insert(it, std::tuple_cat(std::tuple<entity>(new_entity), registry.get_components<ComponentTypes...>(new_entity)));
                    }
                }
            }
        }

        void on_component_removed(entity_registry const& registry, entity entity_removed) override
        {
            auto const it = std::lower_bound(m_entities_components.begin(), m_entities_components.end(), entity_removed,
                [](std::tuple<entity, ComponentTypes*...> const& tuple, entity const& to_find) {
                return to_find < std::get<entity>(tuple);
            });

            if (it != m_entities_components.end() && std::get<entity>(*it) == entity_removed)
            {
                if (!registry.has_components(entity_removed, m_component_ids))
                {
                    m_entities_components.erase(it);
                }
            }
        }
        gtl::span<component_id const> get_component_types() const noexcept { return m_component_ids; }

        component_id const m_component_ids[sizeof...(ComponentTypes)] = { component_id(typeid(ComponentTypes))... };;
        std::vector<std::tuple<entity, ComponentTypes*...>> m_entities_components;
    };

    template<class ... ComponentTypes>
    struct entity_registry::view
    {
        using group_type = group_impl<ComponentTypes...>;
        using iterator = std::tuple<ComponentTypes*...>*;
        using const_iterator = std::tuple<ComponentTypes...> const*;

        view(group_type& group) : m_group(group) {}

        auto begin() const noexcept { return m_group.m_entities_components.begin(); }
        auto cbegin() const noexcept { return m_group.m_entities_components.cbegin(); }

        auto end() const noexcept { return m_group.m_entities_components.end(); }
        auto cend() const noexcept { return m_group.m_entities_components.cend(); }

    private:
        group_type& m_group;
    };

    template<class ... ComponentType>
    [[nodiscard]] entity_registry::view<ComponentType...> entity_registry::get_view()
    {
        using group_type = group_impl<ComponentType...>;
        component_id const group_type_hash = component_id(typeid(group_type));
        {
            std::shared_lock shared_lock(m_lock);
            if (auto it = m_group_map.find(group_type_hash);
                it != m_group_map.end())
            {
                return { *gtl::cast<group_type*>(it->second.get()) };
            }
        }
        
        std::unique_lock lock(m_lock);
        auto const it = m_group_map.insert(std::pair{ group_type_hash, std::unique_ptr<group_base>() });
        if (it.second)
        {
            it.first->second = std::make_unique<group_type>();
            for (auto& entity_comp : m_entity_to_component)
            {
                it.first->second->on_component_added(*this, entity_comp.first);
            }
        }
        return { *gtl::cast<group_type*>(it.first->second.get()) };
    }
}