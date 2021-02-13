#pragma once

#include <string>
#include <vector>
#include "gserializer/serializer.h"

namespace gcore 
{  
    struct node_data_type
    {
        struct vtable
        {
            void (*m_construct_single_element)(void*);
            void* (*m_new_array)(std::size_t);
            void (*m_destroy_single_element)(void*);
            void (*m_delete_array)(void*);
            void (*m_move_single_element)(void*, void*);
            void (*m_copy_single_element)(void const*, void*);
            void* (*m_copy_array)(void const*, std::size_t);
            bool (*m_can_fit_in_buffer)(std::size_t, std::size_t);
            void (*process)(gserializer::serializer&, void* buffer, std::size_t number_of_element);
        };

        using id_type = std::uint16_t;
        static constexpr id_type invalid_id = 0;
        
        node_data_type(std::string name, id_type id, vtable table) : m_vtable(table), m_name(std::move(name)), m_id(id) {}

        std::string const& get_name() const { return m_name; }
        id_type get_id() const { return m_id; }

        
        vtable m_vtable;
    private:
        std::string m_name;
        id_type m_id;
    };

    namespace node_data_vtable
    {
        template<class Type>
        void construct_single_element(void* buffer)
        {
            new(buffer) Type();
        }

        template<class Type>
        void* new_array(std::size_t number_of_elements)
        {
            Type* data = new Type[number_of_elements];
            return static_cast<void*>(data);
        }

        template<class Type>
        void  destroy_single_element(void* buffer)
        {
            reinterpret_cast<Type*>(buffer)->~Type();
        }

        template<class Type>
        void delete_array(void* array)
        {
            delete[] static_cast<Type*>(array);
        }

        template<class Type>
        void move_single_element(void* source_buffer, void* destination_buffer)
        {
            new(destination_buffer) Type(std::move(*reinterpret_cast<Type*>(source_buffer)));
        }

        template<class Type>
        void copy_single_element(void const* source_buffer, void* destination_buffer)
        {
            new(destination_buffer) Type(*reinterpret_cast<Type const*>(source_buffer));
        }

        template<class Type>
        void* copy_array(void const* source, std::size_t number_of_elements)
        {
            Type const* source_data = static_cast<Type const*>(source);
            Type* data = static_cast<Type*>(operator new[](number_of_elements * sizeof(Type)));
            for (size_t i = 0; i < number_of_elements; ++i)
                new(static_cast<void*>(data + i)) Type(*(source_data + i));
            return data;
        }

        template<class Type>
        bool can_fit_in_buffer(std::size_t buffer_size, std::size_t alignement)
        {
            return sizeof(Type) <= buffer_size && alignof(Type) <= alignement;
        }

        template<class Type>
        void process(gserializer::serializer& serializer, void* buffer, std::size_t number_of_element)
        {
            using gserializer::process;
            Type* value = static_cast<Type*>(buffer);
            if (number_of_element == 1)
            {
                process(serializer, *value);
            }
            else
            {
                for (std::size_t i = 0; i < number_of_element; ++i)
                {
                    serializer.open_array_element("value");
                    process(serializer, *(value + i));
                    serializer.close_array_element("value");
                }
            }
        }
    }

    template<class Type>
    node_data_type::vtable generate_vtable()
    {
        return {
            &node_data_vtable::construct_single_element<Type>,
            &node_data_vtable::new_array<Type>,
            &node_data_vtable::destroy_single_element<Type>,
            &node_data_vtable::delete_array<Type>,
            &node_data_vtable::move_single_element<Type>,
            &node_data_vtable::copy_single_element<Type>,
            &node_data_vtable::copy_array<Type>,
            &node_data_vtable::can_fit_in_buffer<Type>,
            &node_data_vtable::process<Type>,
        };
    }

    struct node_data_type_registry
    {
        template<class Type>
        static node_data_type::id_type get_type_id()
        {
            static const node_data_type::id_type id = get_type_id_internal<std::remove_const_t<Type>>();
            return id;
        }

        static node_data_type_registry& get()
        {
            return m_instance;
        }

        template<class Type>
        void register_type(std::string name)
        {
            m_node_data_types.push_back(node_data_type(std::move(name), get_type_id<Type>(), generate_vtable<Type>()));
        }

        node_data_type& get_node_data_type(node_data_type::id_type id)
        {
            return m_node_data_types.at(id - 1);
        }

        node_data_type const* get_by_name(std::string_view name) const;

    private:
        template<class Type>
        static node_data_type::id_type get_type_id_internal()
        {
            static const node_data_type::id_type id = ++get_id_generator();
            return id;
        }

        static node_data_type::id_type& get_id_generator()
        {
            static node_data_type::id_type generator = 0;
            return generator;
        }

        node_data_type_registry() = default;
        static node_data_type_registry m_instance;
        std::vector<node_data_type> m_node_data_types;
    };
}