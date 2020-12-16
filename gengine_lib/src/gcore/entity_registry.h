#pragma once

#include <unordered_map>
#include <vector>
#include <memory>
#include <tuple>
#include <shared_mutex>

#include "gtl/span.h"
#include "gtl/cast.h"

#include "gcore/component.h"

namespace gcore
{

    using entity = std::size_t;
    using component_id = std::size_t;
    using view_id = std::size_t;

    struct entity_registry
    {
        [[nodiscard]] entity create_entity();
        void remove_entity(entity entity);
        
        void add_components(entity entity, gtl::span<std::unique_ptr<component>> components);
        template<class ComponentType>
        void remove_component(entity entity) { remove_component(entity, typeid(ComponentType).hash_code()); }

        template<class ... ComponentType>
        [[nodiscard]]auto get_components(entity entity) const
        {
            if constexpr (sizeof...(ComponentType) == 1)
            {
                return gtl::cast<ComponentType*...>(get_component(entity, typeid(ComponentType...).hash_code()));
            }
            else
            {
                return std::tuple<ComponentType*...>{ gtl::cast<ComponentType*>(get_component(entity, typeid(ComponentType).hash_code()))... };
            }
        }


        template<class ... ComponentType>
        [[nodiscard]] bool has_components(entity) const { 
            static component_id component_types[] = { typeid(ComponentType).hash_code()... };
            return has_component(entity, component_types); 
        }

        template<class ...>
        struct view;

        template<class ... ComponentType>
        [[nodiscard]] view<ComponentType...> get_view();

    private:
        void remove_component(entity entity, component_id component_type);
        bool has_components(entity entity, gtl::span<component_id const> component_types) const;
        component* get_component(entity entity, component_id component_type) const;

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
            std::vector<std::unique_ptr<component>> m_components;
        };
        
        std::unordered_map<entity, std::vector<component_id>> m_entity_to_component_id;
        std::unordered_map<component_id, entity_component_list> m_component_type_map;
        std::unordered_map<view_id, std::unique_ptr<group_base>> m_group_map;
        std::shared_mutex m_lock;
        entity m_entity_generator = 0;
    };

    template<class ... ComponentTypes>
    struct entity_registry::group_impl : entity_registry::group_base
    {

    private:
        friend view;
        friend entity_registry;
        
        void on_component_added(entity_registry const& registry, entity entity) override
        {
            if (auto const iterator_in_entity_vector = std::lower_bound(m_entities.begin(), m_entities.end(), entity);
                iterator_in_entity_vector == m_entities.end() || *iterator_in_entity_vector != entity)
            {
                if (registry.has_components(entity, m_component_ids))
                {
                    auto const diff = std::distance(m_entities.begin(), iterator_in_entity_vector);
                    auto const iterator_in_comp_vector = m_components.begin() + diff;
                    m_components.insert(iterator_in_comp_vector, registry.get_components<ComponentTypes...>(entity));
                    m_entities.insert(iterator_in_entity_vector, entity);
                }
            }
        }

        void on_component_removed(entity_registry const& registry, entity entity) override
        {
            if (auto const iterator_in_entity_vector = std::lower_bound(m_entities.begin(), m_entities.end(), entity);
                iterator_in_entity_vector != m_entities.end() && *iterator_in_entity_vector == entity)
            {
                if (!registry.has_components(entity, m_component_ids))
                {
                    auto const diff = std::distance(m_entities.begin(), iterator_in_entity_vector);
                    auto const iterator_in_comp_vector = m_components.begin() + diff;
                    if (iterator_in_comp_vector != m_components.end())
                    {
                        m_components.erase(iterator_in_comp_vector);
                        m_entities.erase(iterator_in_entity_vector);
                    }
                }
            }
        }
        gtl::span<component_id const> get_component_types() const noexcept { return m_component_ids; }

        component_id const m_component_ids[sizeof...(ComponentTypes)] = { typeid(ComponentTypes).hash_code()... };
        std::vector<entity> m_entities;
        std::vector<std::tuple<ComponentTypes*...>> m_components;
    };

    template<class ... ComponentTypes>
    struct entity_registry::view
    {
        using group_type = group_impl<ComponentTypes...>;
        using iterator = std::tuple<ComponentTypes*...>*;
        using const_iterator = std::tuple<ComponentTypes...> const*;

        view(group_type& group) : m_group(group) {}

        auto begin() const noexcept { return m_group.m_components.begin(); }
        auto cbegin() const noexcept { return m_group.m_components.cbegin(); }

        auto end() const noexcept { return m_group.m_components.end(); }
        auto cend() const noexcept { return m_group.m_components.cend(); }

    private:
        group_type& m_group;
    };

    template<class ... ComponentType>
    [[nodiscard]] entity_registry::view<ComponentType...> entity_registry::get_view()
    {
        using group_type = group_impl<ComponentType...>;
        std::size_t const group_type_hash = typeid(group_type).hash_code();
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
            for (auto& entity_comp : m_entity_to_component_id)
            {
                it.first->second->on_component_added(*this, entity_comp.first);
            }
        }
        return { *gtl::cast<group_type*>(it.first->second.get()) };
    }
}