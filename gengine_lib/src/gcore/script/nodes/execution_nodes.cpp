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
}
