#pragma once

#include <stdafx.h>

#include "gcore/script/node_data.h"

namespace gcore
{
    node_data::node_data(node_data&& move)
        : m_type_id(move.m_type_id)
        , m_is_inline(move.m_is_inline)
    {
        if (move.m_type_id == node_data_type::invalid_id)
            return;

        node_data_type const& data_type = node_data_type_registry::get().get_node_data_type(m_type_id);
        if (move.m_is_inline)
        {
            data_type.m_vtable.m_move_single_element(move.m_inline_info.m_buffer, m_inline_info.m_buffer);
        }
        else
        {
            m_pointer_info = move.m_pointer_info;
            move.m_pointer_info.m_pointer_to_data = nullptr;
            move.m_pointer_info.m_number_of_elements = 0;
        }
    }

    node_data::node_data(node_data const& copy)
        : m_type_id(copy.m_type_id)
        , m_is_inline(copy.m_is_inline)
    {
        if (copy.m_type_id == node_data_type::invalid_id)
            return;

        node_data_type const& data_type = node_data_type_registry::get().get_node_data_type(m_type_id);
        if (copy.m_is_inline)
        {
            data_type.m_vtable.m_copy_single_element(copy.m_inline_info.m_buffer, m_inline_info.m_buffer);
        }
        else
        {
            m_pointer_info.m_pointer_to_data = data_type.m_vtable.m_copy_array(copy.m_pointer_info.m_pointer_to_data, copy.m_pointer_info.m_number_of_elements);
            m_pointer_info.m_number_of_elements = copy.m_pointer_info.m_number_of_elements;
        }
    }

    void node_data::clear()
    {
        if (m_type_id == node_data_type::invalid_id)
            return;

        node_data_type const& data_type = node_data_type_registry::get().get_node_data_type(m_type_id);
        if (m_is_inline)
        {
            data_type.m_vtable.m_destroy_single_element(static_cast<void*>(&m_inline_info.m_buffer));
        }
        else
        {
            data_type.m_vtable.m_delete_array(m_pointer_info.m_pointer_to_data, m_pointer_info.m_number_of_elements);
        }
        m_is_inline = false;
        m_type_id = node_data_type::invalid_id;
    }

    void node_data::set_type_id(node_data_type::id_type type_id)
    {
        if (m_type_id == type_id)
            return;

        clear();
        m_type_id = type_id;
        node_data_type const& data_type = node_data_type_registry::get().get_node_data_type(m_type_id);
        if(data_type.m_vtable.m_can_fit_in_buffer(internal_buffer_size, internal_buffer_alignment))
        {
            m_is_inline = true;
            data_type.m_vtable.m_construct_single_element(static_cast<void*>(&m_inline_info.m_buffer));
        }
        else
        {
            m_is_inline = false;
            m_pointer_info.m_pointer_to_data = data_type.m_vtable.m_new_array(1);
            m_pointer_info.m_number_of_elements = 1;
        }
    }

    void node_data::process(gserializer::serializer& serializer)
    {
        std::string type_name;
        if (m_type_id != node_data_type::invalid_id)
        {
            type_name = node_data_type_registry::get().get_node_data_type(m_type_id).get_name();
        }

        serializer.process("type_name", type_name);
        node_data_type const* new_node_type = node_data_type_registry::get().get_by_name(type_name);
        if (!new_node_type)
        {
            clear();
            return;
        }

        std::size_t number_of_elements = 0;
        if (m_is_inline)
        {
            number_of_elements = 1;
        }
        else
        {
            number_of_elements = m_pointer_info.m_number_of_elements;
        }
        serializer.process("number_of_elements", number_of_elements);


        if (new_node_type->get_id() != m_type_id)
        {
            clear();
            m_type_id = new_node_type->get_id();
            if (number_of_elements == 1 && new_node_type->m_vtable.m_can_fit_in_buffer(internal_buffer_size, internal_buffer_alignment))
            {
                m_is_inline = true;
                new_node_type->m_vtable.m_construct_single_element(m_inline_info.m_buffer);
            }
            else
            {
                m_is_inline = false;
                m_pointer_info.m_pointer_to_data = new_node_type->m_vtable.m_new_array(number_of_elements);
                m_pointer_info.m_number_of_elements = number_of_elements;
            }
        }

        if (m_is_inline)
        {
            new_node_type->m_vtable.m_process(serializer, m_inline_info.m_buffer, 1);
        }
        else
        {
            new_node_type->m_vtable.m_process(serializer, m_pointer_info.m_pointer_to_data, m_pointer_info.m_number_of_elements);
        }
    }

    node_data& node_data::operator=(node_data&& move)
    {
        if (&move == this)
            return *this;

        clear();
        if (move.m_type_id == node_data_type::invalid_id)
            return *this;
        
        m_is_inline = move.m_is_inline;
        node_data_type const& data_type = node_data_type_registry::get().get_node_data_type(m_type_id);
        if (move.m_is_inline)
        {
            data_type.m_vtable.m_move_single_element(move.m_inline_info.m_buffer, m_inline_info.m_buffer);
        }
        else
        {
            m_pointer_info = move.m_pointer_info;
            move.m_pointer_info.m_pointer_to_data = nullptr;
            move.m_pointer_info.m_number_of_elements = 0;
        }
        return *this;
    }

    node_data& node_data::operator=(node_data const& copy)
    {
        if (&copy == this)
            return *this;

        clear();
        if (copy.m_type_id == node_data_type::invalid_id)
            return *this;

        m_type_id = copy.m_type_id;
        m_is_inline = copy.m_is_inline;
        node_data_type const& data_type = node_data_type_registry::get().get_node_data_type(m_type_id);
        if (copy.m_is_inline)
        {
            data_type.m_vtable.m_copy_single_element(copy.m_inline_info.m_buffer, m_inline_info.m_buffer);
        }
        else
        {
            m_pointer_info.m_pointer_to_data = data_type.m_vtable.m_copy_array(copy.m_pointer_info.m_pointer_to_data, copy.m_pointer_info.m_number_of_elements);
            m_pointer_info.m_number_of_elements = copy.m_pointer_info.m_number_of_elements;
        }
        return *this;
    }
}