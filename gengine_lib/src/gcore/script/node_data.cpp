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
            data_type.m_vtable.m_delete_array(m_pointer_info.m_pointer_to_data);
        }
        m_is_inline = false;
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