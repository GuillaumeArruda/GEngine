#include "stdafx.h"

#include <algorithm>
#include <optick/optick.h>

#include "gserializer/serializer.h"

#include "gcore/entity_registry.h"

#include "gcore/resource_library.h"
#include "gcore/serializers/dependency_gatherer_serializer.h"

namespace gcore
{
    entity_registry::entity_registry(std::shared_ptr<resource_library> res_lib) 
        : m_res_library(std::move(res_lib))
    {
        m_resource_reloaded_id = m_res_library->on_resource_reloaded.add_callback([&](gtl::uuid uuid) { on_resource_reloaded(uuid); });
    }

    entity_registry::~entity_registry()
    {
        m_res_library->on_resource_reloaded.remove_callback(m_resource_reloaded_id);
    }

    entity entity_registry::create_entity()
    { 
        return entity::generate();
    }

    void entity_registry::remove_entity(entity entity)
    {
        if (auto const it = m_entity_to_component.find(entity); 
            it != m_entity_to_component.end())
        {
            for (std::unique_ptr<component>& component : it->second)
            {
                remove_component_from_map(entity, component_id(typeid(*component.get())));
            }
            std::vector<std::unique_ptr<component>> holder = std::move(it->second);
            m_entity_to_component.erase(it);
            warn_group_of_component_removal(entity);
        }

        if (auto loading_it = m_loading_component_entity.find(entity);
            loading_it != m_loading_component_entity.end())
        {
            m_loading_component_entity.erase(loading_it);
        }

        m_dependency_tracker.remove_entity(entity);
    }

    void entity_registry::process(gserializer::serializer& serializer)
    {
        serializer.process("Entities", m_entity_to_component, "Entity", "Component", component::factory());
    }

    void entity_registry::remove_component(entity entity, component_id component_type)
    {
        remove_component_from_map(entity, component_type);
        std::unique_ptr<component> holder;
        if (auto it = m_entity_to_component.find(entity);
            it != m_entity_to_component.end())
        {
            auto component_it = std::find_if(it->second.begin(), it->second.end(), [&](std::unique_ptr<component> const& comp) { return component_id(typeid(*comp.get())) == component_type; });
            if (component_it != it->second.end())
            {
                holder = std::move(*component_it);
                it->second.erase(component_it);
            }
        }
        warn_group_of_component_removal(entity);
    }

    void entity_registry::add_loaded_component(entity entity, std::unique_ptr<component> component)
    {
        auto& entity_comp_id_list = m_entity_to_component[entity];
        add_component_to_map(entity, *component);
        entity_comp_id_list.push_back(std::move(component));
    }

    bool entity_registry::has_components(entity entity, gtl::span<component_id const> component_types) const
    {
        if (auto const it = m_entity_to_component.find(entity);
            it != m_entity_to_component.end())
        {
            return std::all_of(component_types.begin(), component_types.end(), [&](component_id comp_type) { return (comp_type == std::type_index(typeid(optional_comp_base))) ||
                std::find_if(it->second.begin(), it->second.end(), [&](std::unique_ptr<component> const& comp) 
            { return comp_type == component_id(typeid(*comp.get())); }) != it->second.end(); });
        }
        return false;
    }

    void entity_registry::add_component_to_map(entity entity_to_add, component& comp)
    {
        entity_component_list& ent_comp_list = m_component_type_map[component_id(typeid(comp))];

        auto const entity_insert_position = std::lower_bound(ent_comp_list.m_entities.begin(), ent_comp_list.m_entities.end(), entity_to_add);
        auto const diff = std::distance(ent_comp_list.m_entities.begin(), entity_insert_position);
        auto const component_insert_position = ent_comp_list.m_components.begin() + diff;

        ent_comp_list.m_entities.insert(entity_insert_position, entity_to_add);
        ent_comp_list.m_components.insert(component_insert_position, &comp);
    }

    void entity_registry::remove_component_from_map(entity entity_to_remove, component_id id)
    {
        if (auto const component_entity_it = m_component_type_map.find(id);
            component_entity_it != m_component_type_map.end())
        {
            if (auto const iterator_in_entity_vector = std::lower_bound(component_entity_it->second.m_entities.begin(), component_entity_it->second.m_entities.end(), entity_to_remove);
                iterator_in_entity_vector != component_entity_it->second.m_entities.end() && *iterator_in_entity_vector == entity_to_remove)
            {
                auto const diff = std::distance(component_entity_it->second.m_entities.begin(), iterator_in_entity_vector);
                auto const iterator_in_comp_vector = component_entity_it->second.m_components.begin() + diff;
                if (iterator_in_comp_vector != component_entity_it->second.m_components.end())
                {
                    component_entity_it->second.m_components.erase(iterator_in_comp_vector);
                    component_entity_it->second.m_entities.erase(iterator_in_entity_vector);
                }
            }
        }
    }

    void entity_registry::warn_group_of_component_addition(entity changed_entity)
    {
        for (auto& group : m_group_map)
        {
            group.second->on_component_added(*this, changed_entity);
        }
    }

    void entity_registry::warn_group_of_component_removal(entity changed_entity)
    {
        for (auto& group : m_group_map)
        {
            group.second->on_component_removed(*this, changed_entity);
        }
    }

    void entity_registry::on_resource_reloaded(gtl::uuid const& resource_uuid)
    {
        gtl::span<entity_dependency_tracker::entity_component const> changed_comp = m_dependency_tracker.get_dependant_components(resource_uuid);

        for (entity_dependency_tracker::entity_component const& entity_comp : changed_comp)
        {
            std::vector<std::unique_ptr<component>>& components = m_entity_to_component[entity_comp.m_entity];
            auto it = std::find_if(components.begin(), components.end(), [&](std::unique_ptr<component> const& comp) { return component_id(typeid(*comp)) == entity_comp.m_component; });
            if (it == components.end())
                continue;
         
            std::unique_ptr<component> holder = std::move(*it);
            components.erase(it);
            warn_group_of_component_removal(entity_comp.m_entity);
            components.push_back(std::move(holder));
            warn_group_of_component_addition(entity_comp.m_entity);
        }
    }

    component* entity_registry::get_component(entity entity, component_id component_type) const
    {
        if (auto const component_entity_it = m_component_type_map.find(component_type);
            component_entity_it != m_component_type_map.end())
        {
            if (auto const iterator_in_entity_vector = std::lower_bound(component_entity_it->second.m_entities.begin(), component_entity_it->second.m_entities.end(), entity);
                iterator_in_entity_vector != component_entity_it->second.m_entities.end() && *iterator_in_entity_vector == entity)
            {
                auto const diff = std::distance(component_entity_it->second.m_entities.begin(), iterator_in_entity_vector);
                auto const iterator_in_comp_vector = component_entity_it->second.m_components.begin() + diff;
                if (iterator_in_comp_vector != component_entity_it->second.m_components.end())
                {
                    return *iterator_in_comp_vector;
                }
            }
        }
        return nullptr;
    }

    gtl::span<std::unique_ptr<component>> entity_registry::see_components(entity entity)
    {
        if (auto const it = m_entity_to_component.find(entity);
            it != m_entity_to_component.end())
        {
            return it->second;
        }
        return gtl::span<std::unique_ptr<component>>();
    }

    bool entity_registry::has_any_component(entity entity) const
    {
        if (auto const it = m_entity_to_component.find(entity);
            it != m_entity_to_component.end())
        {
            return !it->second.empty();
        }
        return false;
    }

    void entity_registry::clear()
    {
        std::unique_lock lock(m_lock);
        m_component_type_map.clear();
        for (auto& group : m_group_map)
        {
            group.second->clear();
        }
        m_entity_to_component.clear();
        m_loading_component_entity.clear();
    }

    void entity_registry::update()
    {
        OPTICK_EVENT();
        std::erase_if(m_loading_component_entity, 
            [&](auto& loading_info) 
            {
                std::size_t const removed_element = 
                    std::erase_if(loading_info.second, 
                        [&](loading_component_info& info)
                        {
                            return std::all_of(info.m_resources.begin(), info.m_resources.end(), [](resource_handle<resource> const& res) { return res.is_loaded(); }) 
                                && (add_loaded_component(loading_info.first, std::move(info.m_component)), true);
                        });
                if (removed_element != 0)
                {
                    warn_group_of_component_addition(loading_info.first);
                }
                return loading_info.second.size() == 0;
            });
    }

    void entity_registry::add_components(entity entity, gtl::span<std::unique_ptr<component>> components)
    {
        bool has_added_component = false;
        gcore::dependency_gatherer_serializer serializer;
        for (auto& component : components)
        {
            if (component)
            {
                serializer.m_uuids.clear();
                serializer.m_files.clear();
                component->process(serializer);

                std::vector<resource_handle<resource>> resources;
                for (gtl::uuid const& res_uuid : serializer.m_uuids)
                {
                    if (resource_handle<resource> res = m_res_library->try_get_resource<resource>(res_uuid))
                    {
                        resources.push_back(std::move(res));
                    }
                }

                m_dependency_tracker.add_dependencies(entity, component_id(typeid(*component.get())), resources);

                if (std::all_of(resources.begin(), resources.end(), [](resource_handle<resource> const& res) { return res.is_loaded(); }))
                {
                    has_added_component = true;
                    add_loaded_component(entity, std::move(component));
                }
                else
                {
                    m_loading_component_entity[entity].push_back({ std::move(component), std::move(resources) });
                }
            }
        }

        if (has_added_component)
        {
            warn_group_of_component_addition(entity);
        }
    }

    void layer_descriptor::process(gserializer::serializer& serializer)
    {
        serializer.process("Entities", m_entity_to_component, "Entity", "Component", component::factory());
    }

}
