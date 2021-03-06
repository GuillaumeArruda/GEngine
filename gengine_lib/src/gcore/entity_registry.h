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
#include "gtl/callbacks.h"

#include "gcore/gcore_defines.h"
#include "gcore/component.h"
#include "gcore/resource_handle.h"
#include "gcore/entity_dependency_tracker.h"

namespace gserializer
{
    struct serializer;
}

namespace gcore
{
    struct optional_comp_base {};

    template<class ComponentType>
    struct optional_comp : optional_comp_base
    {
    };

    namespace details
    {


        template<class ComponentType>
        struct component_type_info
        {
            using component_type = ComponentType;
            inline static const component_id type_id = std::type_index(typeid(ComponentType));
            inline static const component_id has_type_id = std::type_index(typeid(ComponentType));
        };

        template<class ComponentType>
        struct component_type_info<optional_comp<ComponentType>>
        {
            using component_type = ComponentType;
            inline static const component_id type_id = std::type_index(typeid(ComponentType));
            inline static const component_id has_type_id = std::type_index(typeid(optional_comp_base));
        };
    }

    struct layer_descriptor
    {
        std::unordered_map<entity, std::vector<std::unique_ptr<component>>> m_entity_to_component;
        void process(gserializer::serializer& serializer);
    };

    struct entity_registry
    {
        entity_registry(std::shared_ptr<resource_library> res_lib);
        ~entity_registry();

        [[nodiscard]] entity create_entity();
        void remove_entity(entity entity);
        
        void add_components(entity entity, gtl::span<std::unique_ptr<component>> components);
        template<class ComponentType>
        void remove_component(entity entity) { remove_component(entity, details::component_type_info<ComponentType>::type_id); }

        template<class ... ComponentType>
        [[nodiscard]] auto get_components(entity entity) const
        {
            if constexpr (sizeof...(ComponentType) == 1)
            {
                return gtl::cast<typename details::component_type_info<ComponentType>::component_type*...>(get_component(entity, details::component_type_info<ComponentType>::type_id)...);
            }
            else
            {
                return std::tuple<typename details::component_type_info<ComponentType>::component_type*...>{ 
                    gtl::cast<typename details::component_type_info<ComponentType>::component_type*>(get_component(entity, details::component_type_info<ComponentType>::type_id))... };
            }
        }

        template<class ... ComponentType>
        [[nodiscard]] auto get_components(entity entity)
        {
            if constexpr (sizeof...(ComponentType) == 1)
            {
                return gtl::cast<typename details::component_type_info<ComponentType>::component_type*...>(get_component(entity, details::component_type_info<ComponentType>::type_id)...);
            }
            else
            {
                return std::tuple<typename details::component_type_info<ComponentType>::component_type*...>{ 
                    gtl::cast<typename details::component_type_info<ComponentType>::component_type*>(get_component(entity, details::component_type_info<ComponentType>::type_id)... )};
            }
        }


        template<class ... ComponentType>
        [[nodiscard]] bool has_components(entity) const { 
            static component_id const component_types[] = { details::component_type_info<ComponentType>::has_type_id };
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

        void clear();
        void update();
    private:
        bool has_components(entity entity, gtl::span<component_id const> component_types) const;
        
        void add_loaded_component(entity entity, std::unique_ptr<component> component);
        void add_component_to_map(entity entity_to_add, component& component);
        void remove_component_from_map(entity entity_to_remove, component_id id);

        void warn_group_of_component_addition(entity changed_entity);
        void warn_group_of_component_removal(entity changed_entity);

        void on_resource_reloaded(gtl::uuid const& resource_uuid);

        struct group_base
        {
            virtual ~group_base() {}
            virtual void on_component_added(entity_registry const& registry, entity entity) = 0;
            virtual void on_component_removed(entity_registry const& registry, entity entity) = 0;
            virtual gtl::span<component_id const> get_component_types() const noexcept = 0;
            virtual void clear() = 0;
        };

        template<class ... ComponentTypes>
        struct group_impl;
        
        struct entity_component_list
        {
            std::vector<entity> m_entities;
            std::vector<component*> m_components;
        };
        

        struct loading_component_info
        {
            std::unique_ptr<component> m_component;
            std::vector<resource_handle<resource>> m_resources;
        };

        entity_dependency_tracker m_dependency_tracker;
        std::unordered_map<entity, std::vector<loading_component_info>> m_loading_component_entity;
        std::unordered_map<entity, std::vector<std::unique_ptr<component>>> m_entity_to_component;
        std::unordered_map<component_id, entity_component_list> m_component_type_map;
        std::unordered_map<view_id, std::unique_ptr<group_base>> m_group_map;
        std::shared_ptr<resource_library> m_res_library;
        std::shared_mutex m_lock;
        gtl::callback_id m_resource_reloaded_id;
    };


    template<class ... ComponentTypes>
    struct entity_registry::group_impl : entity_registry::group_base
    {

    private:
        friend view;
        friend entity_registry;
        using tuple_type = std::tuple<entity, typename details::component_type_info<ComponentTypes>::component_type*...>;
        void on_component_added(entity_registry const& registry, entity new_entity) override
        {
            if (registry.has_components(new_entity, m_component_ids))
            {
                auto const it = std::lower_bound(m_entities_components.begin(), m_entities_components.end(), new_entity,
                    [](tuple_type const& tuple, entity const& to_find) {
                    return to_find < std::get<entity>(tuple);
                });

                if (it == m_entities_components.end() || std::get<entity>(*it) != new_entity)
                {
                     auto new_value = m_entities_components.insert(it, get_components_entity_tuple(registry, new_entity));
                     m_on_added(*new_value);
                }
                else
                {
                    *it = get_components_entity_tuple(registry, new_entity); // in case an optional comp has been added
                }
            }
        }

        void on_component_removed(entity_registry const& registry, entity entity_removed) override
        {
            auto const it = std::lower_bound(m_entities_components.begin(), m_entities_components.end(), entity_removed,
                [](tuple_type  const& tuple, entity const& to_find) {
                return to_find < std::get<entity>(tuple);
            });

            if (it != m_entities_components.end() && std::get<entity>(*it) == entity_removed)
            {
                if (!registry.has_components(entity_removed, m_component_ids) || !registry.has_any_component(entity_removed))
                {
                    m_on_removed(*it);
                    m_entities_components.erase(it);
                }
                else
                {
                    *it = get_components_entity_tuple(registry, entity_removed); // in case an optional comp got removed
                }
            }
        }

        void clear()
        {
            for (auto& tuple : m_entities_components)
            {
                m_on_removed(tuple);
            }
            m_entities_components.clear();
        }

        gtl::span<component_id const> get_component_types() const noexcept { return m_component_ids; }

        component_id const m_component_ids[sizeof...(ComponentTypes)] = { details::component_type_info<ComponentTypes>::has_type_id... };;
        std::vector<tuple_type > m_entities_components;
        gtl::callbacks<void (tuple_type&)> m_on_added;
        gtl::callbacks<void (tuple_type&)> m_on_removed;
    private:
        tuple_type get_components_entity_tuple(entity_registry const& registry, entity ent) const
        {
            if constexpr (sizeof...(ComponentTypes) == 1)
            {
                return std::tuple(ent, registry.get_components<ComponentTypes...>(ent));
            }
            else
            {
                return std::tuple_cat(std::tuple<entity>(ent), registry.get_components<ComponentTypes...>(ent));
            }
        }

    };

    template<class ... ComponentTypes>
    struct entity_registry::view
    {
        using group_type = group_impl<ComponentTypes...>;
        using iterator = typename group_type::tuple_type*;
        using const_iterator = typename group_type::tuple_type const*;

        view(group_type& group) : m_group(group) {}

        auto begin() const noexcept { return m_group.m_entities_components.begin(); }
        auto cbegin() const noexcept { return m_group.m_entities_components.cbegin(); }

        auto end() const noexcept { return m_group.m_entities_components.end(); }
        auto cend() const noexcept { return m_group.m_entities_components.cend(); }

        template<class CallbackTypeObject>
        gtl::callback_id add_on_added_callback(CallbackTypeObject&& callback)
        {
            return m_group.m_on_added.add_callback(std::forward<CallbackTypeObject>(callback));
        }

        template<class CallbackTypeObject>
        gtl::callback_id add_on_removed_callback(CallbackTypeObject&& callback)
        {
            return m_group.m_on_removed.add_callback(std::forward<CallbackTypeObject>(callback));
        }

        void remove_on_added_callback(gtl::callback_id id)
        {
            m_group.m_on_added.remove_callback(id);
        }

        void remove_on_removed_callback(gtl::callback_id id)
        {
            m_group.m_on_removed.remove_callback(id);
        }


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