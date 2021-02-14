#include "stdafx.h"
#include "gcore/script/script_context.h"

namespace gcore
{
    node_context::node_context(script_context& context, gtl::span<in_pin_data> input_data, gtl::span<node_data> output_data)
        : m_script_context(&context)
        , m_input_data(input_data)
        , m_output_data(output_data)
    {
    }

    node_context::node_context(script_context& context, node_context const& copy, char*& memory_location)
        : m_script_context(&context)
    {       
        char* const input_beginning = memory_location;
        for (in_pin_data const& input: copy.m_input_data)
        {
            new(memory_location) in_pin_data(input);
            memory_location += sizeof(in_pin_data);
        }
        m_input_data = gtl::span<in_pin_data>(reinterpret_cast<in_pin_data*>(input_beginning), copy.m_input_data.size());

        char* const output_beginning = memory_location;
        for (node_data const& output : copy.m_output_data)
        {
            new(memory_location) node_data(output);
            memory_location += sizeof(node_data);
        }
        m_output_data = gtl::span<node_data>(reinterpret_cast<node_data*>(output_beginning), copy.m_output_data.size());
    }

    node_context::~node_context()
    {
        clear();
    }

    node_context::node_context(node_context&& move) noexcept
        : m_script_context(std::move(move.m_script_context))
        , m_output_data(std::move(move.m_output_data))
        , m_input_data(std::move(move.m_input_data))
    {
        move.m_output_data = gtl::span<node_data>();
        move.m_input_data = gtl::span<in_pin_data>();
    }

    node_context& node_context::operator=(node_context&& move) noexcept
    {
        if(&move == this)
            return *this;

        clear();
        m_script_context = std::move(move.m_script_context);
        m_output_data = std::move(move.m_output_data);
        m_input_data = std::move(move.m_input_data);


        move.m_output_data = gtl::span<node_data>();
        move.m_input_data = gtl::span<in_pin_data>();
        return *this;
    }

    void node_context::clear()
    {
        for (node_data& data : m_output_data)
            data.~node_data();

        for (in_pin_data& data : m_input_data)
            data.~in_pin_data();
    }

    void node_context::evaluate_input(std::uint32_t index) const
    {
        m_script_context->execute_node(index);
    }

    script_context::script_context(script_context const& copy)
        : m_script(copy.m_script)
        , m_memory_buffer(std::make_unique<char[]>(m_script->get_necessary_memory_for_context()))
        
    {
        char* location = m_memory_buffer.get();
        m_node_contexts.reserve(copy.m_node_contexts.size());
        for (node_context const& context : copy.m_node_contexts)
        {
            m_node_contexts.emplace_back(*this, context, location);
        }
    }

    script_context::script_context(script_context&& move) noexcept
        : m_script(move.m_script)
        , m_memory_buffer(std::move(move.m_memory_buffer))
        , m_node_contexts(std::move(move.m_node_contexts))
    {
        move.m_script = nullptr;
    }

    script_context& script_context::operator=(script_context const& copy)
    {
        if (this == &copy)
            return *this;

        m_node_contexts.clear();
        m_memory_buffer.reset();

        m_script = copy.m_script;
        m_memory_buffer = std::make_unique<char[]>(m_script->get_necessary_memory_for_context());
        char* location = m_memory_buffer.get();
        m_node_contexts.reserve(copy.m_node_contexts.size());
        for (node_context const& context : copy.m_node_contexts)
        {
            m_node_contexts.emplace_back(*this, context, location);
        }

        return *this;
    }

    script_context& script_context::operator=(script_context&& move)
    {
        if (this == &move)
            return *this;

        m_node_contexts = std::move(move.m_node_contexts);
        m_memory_buffer = std::move(move.m_memory_buffer);
        m_script = move.m_script;
        return *this;
    }

    void script_context::execute()
    {
        for (std::uint32_t root_index : m_script->get_root_node_indexes())
        {
            execute_node(root_index);
        }
    }

    void script_context::execute_node(std::uint32_t node_index)
    {
        assert(node_index < m_node_contexts.size());
        m_script->get_node(node_index)->execute(m_node_contexts[node_index]);
    }
}

