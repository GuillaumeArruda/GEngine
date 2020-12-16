#include "stdafx.h"
#include <algorithm>

#include "gserializer/serializer.h"

#include "gcore/entity_registry.h"


namespace gcore
{
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
                remove_component(entity, component_id(typeid(*component.get())));
            }
            m_entity_to_component.erase(it);
        }
    }

    void entity_registry::process(gserializer::serializer& serializer)
    {
        serializer.process("entities", m_entity_to_component, component::factory());
        if (serializer.is_writing_to_object())
        {
            rebuild_component_type_map();
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
            if (auto it = m_entity_to_component.find(entity);
                it != m_entity_to_component.end())
            {
                it->second.erase(std::find_if(it->second.begin(), it->second.end(), [&](std::unique_ptr<component> const& comp) { return component_id(typeid(*comp.get())) == component_type; }));
            }
        }

        for (auto& group : m_group_map)
        {
            group.second->on_component_removed(*this, entity);
        }
    }

    bool entity_registry::has_components(entity entity, gtl::span<component_id const> component_types) const
    {
        if (auto const it = m_entity_to_component.find(entity);
            it != m_entity_to_component.end())
        {
            return std::all_of(component_types.begin(), component_types.end(), [&](component_id comp_type) { return std::find_if(it->second.begin(), it->second.end(), [&](std::unique_ptr<component> const& comp) { return comp_type == component_id(typeid(*comp.get())); }) != it->second.end(); });
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
                    return *iterator_in_comp_vector;
                }
            }
        }
        return nullptr;
    }

    void entity_registry::rebuild_component_type_map()
    {
        m_component_type_map.clear();
        m_group_map.clear();

        for (auto& entity : m_entity_to_component)
        {
            entity.second.erase(
                std::remove_if(entity.second.begin(), entity.second.end(),
                [&](auto& comp) 
                {
                    if (comp)
                    {
                        entity_component_list& ent_comp_list = m_component_type_map[component_id(typeid((*comp.get())))];

                        auto const entity_insert_position = std::lower_bound(ent_comp_list.m_entities.begin(), ent_comp_list.m_entities.end(), entity.first);
                        auto const diff = std::distance(ent_comp_list.m_entities.begin(), entity_insert_position);
                        auto const component_insert_position = ent_comp_list.m_components.begin() + diff;

                        ent_comp_list.m_entities.insert(entity_insert_position, entity.first);
                        ent_comp_list.m_components.insert(component_insert_position, comp.get());
                        return false;
                    }
                    return true;
                }
            ), entity.second.end());
            for (auto& comp : entity.second)
            {
               
            }
        }
    }

    void entity_registry::add_components(entity entity, gtl::span<std::unique_ptr<component>> components)
    {
        auto& entity_comp_id_list = m_entity_to_component[entity];
        for (auto& component : components)
        {
            if (component)
            {
                entity_component_list& ent_comp_list = m_component_type_map[component_id(typeid((*component.get())))];

                auto const entity_insert_position = std::lower_bound(ent_comp_list.m_entities.begin(), ent_comp_list.m_entities.end(), entity);
                auto const diff = std::distance(ent_comp_list.m_entities.begin(), entity_insert_position);
                auto const component_insert_position = ent_comp_list.m_components.begin() + diff;

                ent_comp_list.m_entities.insert(entity_insert_position, entity);
                ent_comp_list.m_components.insert(component_insert_position, component.get());
                entity_comp_id_list.push_back(std::move(component));
            }

        }
        for (auto& group : m_group_map)
        {
            group.second->on_component_added(*this, entity);
        }
    }

}
