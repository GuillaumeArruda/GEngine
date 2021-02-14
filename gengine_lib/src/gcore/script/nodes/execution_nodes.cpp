#include "stdafx.h"
#include "execution_nodes.h"

namespace gcore
{
    root_execution_node::root_execution_node()
    {
        m_descriptors.resize(1);
        fill_descriptors();
    }

    void root_execution_node::execute(node_context& context) const
    {
        for (std::size_t i = 0; i < m_descriptors.size(); ++i)
        {
            context.read<execution_pin_data>(static_cast<int>(i));
        }
    }

    node::pin_descriptors root_execution_node::get_pin_descriptors() const
    {
        return { m_descriptors, {} };
    }

    void root_execution_node::on_input_pin_state_changed(int pin_id, pin_state state)
    {
        if (state == pin_state::connected)
        {
            if (pin_id == m_descriptors.size() - 1)
            {
                m_descriptors.emplace_back();
                fill_descriptors();
            }
        }
        else
        {
            if(pin_id == m_descriptors.size() - 2)
                m_descriptors.pop_back();
        }
    }

    void root_execution_node::process(gserializer::serializer& serializer)
    {
        node::process(serializer);
        std::size_t number_of_inputs = m_descriptors.size();;
        serializer.process("connected_inputs", number_of_inputs);
        if (number_of_inputs != m_descriptors.size())
        {
            m_descriptors.resize(number_of_inputs);
            fill_descriptors();
        }
    }

    void root_execution_node::fill_descriptors()
    {
        for (std::size_t i = 0; i < m_descriptors.size(); ++i)
        {
            m_descriptors[i].m_id = static_cast<int>(i);
            m_descriptors[i].m_index = static_cast<int>(i);
            m_descriptors[i].m_type_id = node_data_type_registry::get_type_id<execution_pin_data>();
            m_descriptors[i].m_is_optional = true;
            m_descriptors[i].m_pin_type = pin_type::input;
            m_descriptors[i].m_name = "execute";
        }
    }

    sequence_execution_node::sequence_execution_node()
    {
        m_descriptors.resize(1);
        fill_descriptors();
    }

    void sequence_execution_node::execute(node_context& context) const
    {
        for (std::size_t i = 0; i < m_descriptors.size(); ++i)
        {
            context.read<execution_pin_data>(static_cast<int>(i));
        }
        context.create_array<out_pin>(1);
    }

    node::pin_descriptors sequence_execution_node::get_pin_descriptors() const
    {
        static pin_descriptor const outputs[] = { out_pin::get("execute") };
        return { m_descriptors, outputs };
    }

    void sequence_execution_node::on_input_pin_state_changed(int pin_id, pin_state state)
    {
        if (state == pin_state::connected)
        {
            if (pin_id == m_descriptors.size() - 1)
            {
                m_descriptors.emplace_back();
                fill_descriptors();
            }
        }
        else
        {
            if (pin_id == m_descriptors.size() - 2)
                m_descriptors.pop_back();
        }
    }

    void sequence_execution_node::process(gserializer::serializer& serializer)
    {
        node::process(serializer);
        std::size_t number_of_inputs = m_descriptors.size();;
        serializer.process("connected_inputs", number_of_inputs);
        if (number_of_inputs != m_descriptors.size())
        {
            m_descriptors.resize(number_of_inputs);
            fill_descriptors();
        }
    }

    void sequence_execution_node::fill_descriptors()
    {
        for (std::size_t i = 0; i < m_descriptors.size(); ++i)
        {
            m_descriptors[i].m_id = static_cast<int>(i);
            m_descriptors[i].m_index = static_cast<int>(i);
            m_descriptors[i].m_type_id = node_data_type_registry::get_type_id<execution_pin_data>();
            m_descriptors[i].m_is_optional = true;
            m_descriptors[i].m_pin_type = pin_type::input;
            m_descriptors[i].m_name = "execute";
        }
    }

    void select_execution_node::execute(node_context& context) const
    {
        gtl::span<bool const> condition = context.read<condition_pin>();
        assert(condition.size() == 1);
        if (condition[0])
        {
            context.read<true_pin>();
        }
        else
        {
            context.read<false_pin>();
        }
        context.create_array<out_pin>(1);
    }

    node::pin_descriptors select_execution_node::get_pin_descriptors() const
    {
        static pin_descriptor const inputs[] = {
            condition_pin::get("condition"),
            true_pin::get("true"),
            false_pin::get("false")
        };
        static pin_descriptor const outputs[] =
        {
            out_pin::get("execute")
        };
        return { inputs, outputs };
    }
}
