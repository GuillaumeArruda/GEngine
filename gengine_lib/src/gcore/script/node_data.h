#pragma once

#include "gtl/span.h"

#include "gcore/script/node_data_type.h"

namespace gcore
{
    struct alignas(16) node_data
    {
        node_data() = default;
        ~node_data() { clear(); }
        node_data(node_data&& move);
        node_data(node_data const& copy);

        node_data& operator=(node_data&& move);
        node_data& operator=(node_data const& copy);

        template<class Type>
        gtl::span<Type const> read() const
        {
            assert(m_type_id == node_data_type_registry::get_type_id<Type>());
            if (m_type_id == node_data_type_registry::get_type_id<Type>())
            {
                if (m_is_inline)
                {
                    return { *reinterpret_cast<Type const*>(&m_inline_info.m_buffer) };
                }
                else
                {
                    return { static_cast<Type const*>(m_pointer_info.m_pointer_to_data), m_pointer_info.m_number_of_elements };
                }
            }
            return {};
        }

        template<class Type>
        gtl::span<Type> read()
        {
            gtl::span<Type const> const_version = static_cast<node_data const*>(this)->read<Type>();
            return { const_cast<Type*>(const_version.data()), const_version.size() };
        }

        template<class Type>
        gtl::span<Type> create_array(std::size_t number_of_element)
        {
            clear();
            m_type_id = node_data_type_registry::get_type_id<Type>();
            if (number_of_element == 0)
                return {};

            node_data_type const& data_type = node_data_type_registry::get().get_node_data_type(m_type_id);
            if (number_of_element == 1 && sizeof(Type) <= internal_buffer_size && alignof(Type) <= internal_buffer_alignment)
            {
                m_is_inline = true;
                data_type.m_vtable.m_construct_single_element(&m_inline_info.m_buffer);
                return { *reinterpret_cast<Type*>(&m_inline_info.m_buffer) };
            }
            else
            {
                m_is_inline = false;
                m_pointer_info.m_pointer_to_data = data_type.m_vtable.m_new_array(number_of_element);
                m_pointer_info.m_number_of_elements = number_of_element;
                return { static_cast<Type*>(m_pointer_info.m_pointer_to_data), m_pointer_info.m_number_of_elements };
            }
        }

        template<class Type>
        gtl::span<Type> write_values(gtl::span<Type const> values)
        {
            clear();
            m_type_id = node_data_type_registry::get_type_id<Type>();
            if (values.size() == 0)
                return {};

            node_data_type const& data_type = node_data_type_registry::get().get_node_data_type(m_type_id);
            if (values.size() == 1 && sizeof(Type) <= internal_buffer_size && alignof(Type) <= internal_buffer_alignment)
            {
                m_is_inline = true;
                data_type.m_vtable.m_copy_single_element(static_cast<void const*>(values.data()), &m_inline_info.m_buffer);
                return { *reinterpret_cast<Type*>(&m_inline_info.m_buffer) };
            }
            else
            {
                m_is_inline = false;
                m_pointer_info.m_pointer_to_data = data_type.m_vtable.m_copy_array(static_cast<void const*>(values.data()), values.size());
                m_pointer_info.m_number_of_elements = values.size();
                return { static_cast<Type*>(m_pointer_info.m_pointer_to_data), m_pointer_info.m_number_of_elements };
            }
        }

        void clear();

        node_data_type::id_type get_type_id() const { return m_type_id; }
        void set_type_id(node_data_type::id_type type_id);

        void process(gserializer::serializer& serializer);
    private:
        static constexpr std::size_t internal_buffer_size = 24;
        static constexpr std::size_t internal_buffer_alignment = 16;
        struct pointer_info
        {
            void* m_pointer_to_data;
            std::size_t m_number_of_elements;
        };
        struct inline_info
        {
            char m_buffer[internal_buffer_size];
        };
        union
        {
             inline_info m_inline_info;
             pointer_info m_pointer_info;
        };
        node_data_type::id_type m_type_id = node_data_type::invalid_id;
        bool m_is_inline : 1 = false;
    };
}