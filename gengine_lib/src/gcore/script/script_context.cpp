#include "stdafx.h"
#include "gcore/script/script_context.h"

#include "gcore/script/node_factory.h"

#include <optick/optick.h>
#include <fmt/format.h>

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

    node_data const* node_context::get_input_node_data(std::size_t index) const
    {
        in_pin_data const& data = m_input_data[index];
        if (data.m_is_optional && data.m_diff_to_node_data == 0)
            return nullptr;

        if (!data.m_has_been_written || !data.m_is_constant)
        {
            evaluate_input(data.m_node_index);
            data.m_has_been_written = true;
        }
        return &m_input_data[index].get_node_data();
    }

    node_data* node_context::get_variable(gtl::uuid const& id)
    {
        return m_script_context->get_variable(id);
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
        copy_variables(copy, location);
        
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
        for (node_context& context : m_node_contexts)
            context.set_context(this);
    }

    script_context::~script_context()
    {
        destroy_variables();
    }

    script_context& script_context::operator=(script_context const& copy)
    {
        if (this == &copy)
            return *this;

        m_node_contexts.clear();
        m_memory_buffer.reset();
        destroy_variables();

        m_script = copy.m_script;
        m_memory_buffer = std::make_unique<char[]>(m_script->get_necessary_memory_for_context());
        char* location = m_memory_buffer.get();
        copy_variables(copy, location);
        m_node_contexts.reserve(copy.m_node_contexts.size());
        for (node_context const& context : copy.m_node_contexts)
        {
            m_node_contexts.emplace_back(*this, context, location);
        }

        m_has_been_prepared = false;
        return *this;
    }

    script_context& script_context::operator=(script_context&& move)
    {
        if (this == &move)
            return *this;

        destroy_variables();

        m_node_contexts = std::move(move.m_node_contexts);
        m_memory_buffer = std::move(move.m_memory_buffer);
        m_script = move.m_script;
        m_has_been_prepared = move.m_has_been_prepared;
        for (node_context& context : m_node_contexts)
            context.set_context(this);

        return *this;
    }

    void script_context::execute()
    {
        if (!m_script)
            return;

        ++m_execution_id;
        for (std::uint32_t root_index : m_script->get_root_node_indexes())
        {
            execute_node(root_index);
        }
    }

    void script_context::execute_node(std::uint32_t node_index)
    {
        assert(node_index < m_node_contexts.size());
        node const* node = m_script->get_node(node_index);
        node_context& node_context = m_node_contexts[node_index];
        if (node_context.m_last_execution_id != m_execution_id)
        {
            node_context.m_last_execution_id = m_execution_id;
#if GCORE_ENABLE_HEAVY_SCRIPT_PROFILE()
            OPTICK_EVENT_DYNAMIC(typeid(*node).name());
            OPTICK_TAG("Node Id", node->get_node_id());
#endif //GCORE_ENABLE_HEAVY_SCRIPT_PROFILE()
            node->execute(node_context);
        }
    }

    void script_context::prepare()
    {
        for (std::uint32_t i =0; i < m_node_contexts.size(); ++i)
        {
            m_script->get_node(i)->prepare(m_node_contexts[i]);
        }
        m_has_been_prepared = true;
    }

    node_data* script_context::get_variable(gtl::uuid const& id)
    {
        gtl::span<script::variable const> vars = m_script->get_variables();
        if (auto it = std::find_if(vars.begin(), vars.end(), [&](auto& var) { return var.m_id == id; });
            it != vars.end())
        {
            return reinterpret_cast<node_data*>(m_memory_buffer.get() + it->m_offset);
        }
        return nullptr;
    }

    void script_context::destroy_variables()
    {
        if (m_memory_buffer)
        {
            char* const start_location = m_memory_buffer.get();
            for (auto const& var : m_script->get_variables())
            {
                reinterpret_cast<node_data*>(start_location + var.m_offset)->~node_data();
            }
        }
    }
    void script_context::copy_variables(script_context const& copy, char*& location)
    {
        for (auto const& var : m_script->get_variables())
        {
            char* var_location = copy.m_memory_buffer.get() + var.m_offset;
            new(location) node_data(*reinterpret_cast<const node_data*>(var_location));
            location += sizeof(node_data);
        }
    }
}

