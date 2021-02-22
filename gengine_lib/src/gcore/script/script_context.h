#pragma once

#include "gtl/span.h"
#include "gtl/any_map.h"

#include "gcore/script/node_data.h"

#include "gcore/resource_handle.h"
#include "gcore/script/script.h"

namespace gcore
{
    struct pin_descriptor
    {
        const char* m_name = nullptr;
        int m_index = 0;
        int m_id = 0;
        node_data_type::id_type m_type_id = node_data_type::invalid_id;
        pin_type m_pin_type = pin_type::input;
        bool m_is_optional = false;
        bool operator==(int id) const { return m_id == id; }
    };

    template<class DataType, int Index, int Id, bool IsOptional = false>
    struct input_pin_descriptor
    {
        using type = DataType;
        static constexpr int index = Index;
        static constexpr int id = Id;
        static constexpr pin_type pin_type = pin_type::input;
        static constexpr bool is_optional = IsOptional;
        static pin_descriptor get(const char* name)
        {
            return pin_descriptor{ name, index, id, node_data_type_registry::get_type_id<type>(), pin_type, is_optional };
        }
    };

    template<class DataType, int Index, int Id, bool IsOptional = false>
    struct output_pin_descriptor
    {
        using type = DataType;
        static constexpr int index = Index;
        static constexpr int id = Id;
        static constexpr pin_type pin_type = pin_type::output;
        static constexpr bool is_optional = IsOptional;
        static pin_descriptor get(const char* name)
        {
            return pin_descriptor{ name, index, id, node_data_type_registry::get_type_id<type>(), pin_type, is_optional};
        }
    };

    struct in_pin_data
    {
        node_data const& get_node_data() const { return *reinterpret_cast<node_data const*>(reinterpret_cast<char const*>(this) + m_diff_to_node_data); }
        std::ptrdiff_t m_diff_to_node_data = 0;
        std::uint32_t m_node_index = 0;
        mutable bool m_has_been_written : 1 = false;
        bool m_is_constant : 1 = false;
        bool m_is_optional : 1 = false;
    };

    struct script_context;

    struct node_context
    {
        node_context(script_context& context, gtl::span<in_pin_data> input_data, gtl::span<node_data> output_data);
        node_context(script_context& context, node_context const& copy, char*& memory_location);
        ~node_context();

        node_context(node_context const&) = delete;
        node_context& operator=(node_context const&) = delete;

        node_context(node_context&& move) noexcept;
        node_context& operator=(node_context&& move) noexcept;

        template<class PinDescriptor>
        gtl::span<typename PinDescriptor::type const> read() const
        {
            static_assert(PinDescriptor::pin_type == gcore::pin_type::input, "Trying to read a non input pin type");
            return read<typename PinDescriptor::type>(PinDescriptor::index);
        }

        template<class Type>
        gtl::span<Type const> read(std::size_t index) const
        {
            in_pin_data const& data = m_input_data[index];
            if (data.m_is_optional && data.m_diff_to_node_data == 0)
                return {};

            if (!data.m_has_been_written || !data.m_is_constant)
            {
                evaluate_input(data.m_node_index);
                data.m_has_been_written = true;
            }
            return m_input_data[index].get_node_data().read<Type const>();
        }

        template<class PinDescriptor>
        gtl::span<typename PinDescriptor::type> create_array(std::size_t number_of_elements) const
        {
            static_assert(PinDescriptor::pin_type == gcore::pin_type::output, "Trying to write a non output pin type");
            return m_output_data[PinDescriptor::index].create_array<typename PinDescriptor::type>(number_of_elements);
        }

        template<class PinDescriptor> 
        gtl::span<typename PinDescriptor::type> write_values(gtl::span<typename PinDescriptor::type const> values)
        {
            static_assert(PinDescriptor::pin_type == gcore::pin_type::output, "Trying to write a non output pin type");
            return m_output_data[PinDescriptor::index].write_values<typename PinDescriptor::type>(values);
        }

        void copy_data_to_output(node_data const& data, int output_index)
        {
            m_output_data[output_index] = data;
        }

        template<class ContextElement>
        void set_in_context(ContextElement&& element);

        template<class ContextElement>
        ContextElement* get_in_context();

        friend script;
    private:
        void clear();
        void evaluate_input(std::uint32_t index) const;
        script_context* m_script_context;
        gtl::span<in_pin_data> m_input_data;
        gtl::span<node_data> m_output_data;
        std::any m_node_data;
    };

    struct script_context
    {
        script_context() = default;
        script_context(script_context const& copy);
        script_context(script_context&& move) noexcept;

        script_context& operator=(script_context const& copy);
        script_context& operator=(script_context&& move);

        void execute();
        void execute_node(std::uint32_t);
        friend script;

        template<class ContextElement>
        void set_in_context(ContextElement&& element)
        {
            m_context.set(std::forward<ContextElement>(element));
        }

        template<class ContextElement>
        ContextElement* get_in_context()
        {
            return m_context.get<ContextElement>();
        }

    private:
        script* m_script = nullptr;
        std::unique_ptr<char[]> m_memory_buffer;
        std::vector<node_context> m_node_contexts;
        gtl::any_map m_context;
    };

    template<class ContextElement>
    void node_context::set_in_context(ContextElement&& element)
    {
        m_script_context->set_in_context(std::forward<ContextElement>(element));
    }

    template<class ContextElement>
    ContextElement* node_context::get_in_context()
    {
        return m_script_context->get_in_context<ContextElement>();
    }
}