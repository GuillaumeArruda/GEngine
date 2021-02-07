#pragma once

#include <string>
#include <vector>

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
        };
    }

    struct node_data_type_registry
    {
        template<class Type>
        static node_data_type::id_type get_type_id()
        {
            static const node_data_type::id_type id = ++get_id_generator();
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

    private:
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