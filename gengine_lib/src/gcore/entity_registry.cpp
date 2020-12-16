#include "stdafx.h"

#include "gcore/entity_registry.h"

namespace gcore
{
    entity entity_registry::create_entity() 
    { 
        return entity::generate();
    }

    void entity_registry::remove_entity(entity entity)
    {
        if (auto const it = m_entity_to_component_id.find(entity); 
            it != m_entity_to_component_id.end())
        {
            for (component_id component_id : it->second)
            {
                remove_component(entity, component_id);
            }
            m_entity_to_component_id.erase(it);
        }
    }

    void entity_registry::remove_component(entity entity, component_id component_type)
    {
        if (auto const component_entity_it = m_component_type_map.find(component_type);
            component_entity_it != m_component_type_map.end())
        {
            if (auto const iterator_in_entity_vector = std::lower_bound(component_entity_it->second.m_entities.begin(), component_entity_it->second.m_entities.end(), entity); 
                *iterator_in_entity_vector == entity)
            {
                auto const diff = std::distance(component_entity_it->second.m_entities.begin(), iterator_in_entity_vector);
                auto const iterator_in_comp_vector = component_entity_it->second.m_components.begin() + diff;
                if (iterator_in_comp_vector != component_entity_it->second.m_components.end())
                {
                    component_entity_it->second.m_components.erase(iterator_in_comp_vector);
                    component_entity_it->second.m_entities.erase(iterator_in_entity_vector);
                }
            }
            if (auto it = m_entity_to_component_id.find(entity);
                it != m_entity_to_component_id.end())
            {
                it->second.erase(std::find(it->second.begin(), it->second.end(), component_type));
            }
        }

        for (auto& group : m_group_map)
        {
            group.second->on_component_removed(*this, entity);
        }
    }

    bool entity_registry::has_components(entity entity, gtl::span<component_id const> component_types) const
    {
        if (auto const it = m_entity_to_component_id.find(entity);
            it != m_entity_to_component_id.end())
        {
            return std::all_of(component_types.begin(), component_types.end(), [&](component_id comp_type) { return std::find(it->second.begin(), it->second.end(), comp_type) != it->second.end(); });
        }
        return false;
    }

    component* entity_registry::get_component(entity entity, component_id component_type) const
    {
        if (auto const component_entity_it = m_component_type_map.find(component_type);
            component_entity_it != m_component_type_map.end())
        {
            if (auto const iterator_in_entity_vector = std::lower_bound(component_entity_it->second.m_entities.begin(), component_entity_it->second.m_entities.end(), entity);
                *iterator_in_entity_vector == entity)
            {
                auto const diff = std::distance(component_entity_it->second.m_entities.begin(), iterator_in_entity_vector);
                auto const iterator_in_comp_vector = component_entity_it->second.m_components.begin() + diff;
                if (iterator_in_comp_vector != component_entity_it->second.m_components.end())
                {
                    return iterator_in_comp_vector->get();
                }
            }
        }
        return nullptr;
    }

    void entity_registry::add_components(entity entity, gtl::span<std::unique_ptr<component>> components)
    {
        auto& entity_comp_id_list = m_entity_to_component_id[entity];
        for (auto& component : components)
        {
            if (component)
            {
                component_id const id = component_id(typeid((*component.get())));
                entity_comp_id_list.push_back(id);

                entity_component_list& ent_comp_list = m_component_type_map[id];

                auto const entity_insert_position = std::lower_bound(ent_comp_list.m_entities.begin(), ent_comp_list.m_entities.end(), entity);
                auto const diff = std::distance(ent_comp_list.m_entities.begin(), entity_insert_position);
                auto const component_insert_position = ent_comp_list.m_components.begin() + diff;

                ent_comp_list.m_entities.insert(entity_insert_position, entity);
                ent_comp_list.m_components.insert(component_insert_position, std::move(component));
            }

        }
        for (auto& group : m_group_map)
        {
            group.second->on_component_added(*this, entity);
        }
    }

}
