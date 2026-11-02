#include "stdafx.h"
#include "gcore/script/nodes/run_script_node.h"

namespace gcore
{
    void run_script_node::prepare(node_context& context) const
    {
        if (m_script.is_loaded())
        {
            state state
            {
                m_script->create_context(),
                m_variables_to_update
            };
            state.m_context.copy_context(context.get_script_context());
            context.m_node_memory = std::move(state);
        }
    }

    void run_script_node::execute(node_context& context) const
    {
        if (m_script.is_loaded())
        {
            auto enables = context.read<in_enable_pin>();

            if (!std::any_of(enables.begin(), enables.end(), [](bool enable) { return enable; }))
                return;

            state& state = std::any_cast<run_script_node::state&>(context.m_node_memory);
            state.m_context.copy_context(context.get_script_context());

            for (auto& variable_update : state.m_variables_to_update)
            {
                if (variable_update.m_update_once && variable_update.m_has_updated)
                    continue;

                variable_update.m_has_updated = true;
                if (node_data const* input_data = context.get_input_node_data(variable_update.m_index))
                {
                    if (node_data* variable = state.m_context.get_variable(variable_update.m_id))
                    {
                        if (input_data->get_type_id() == variable->get_type_id())
                        {
                            *variable = *input_data;
                        }
                    }
                }
            }

            if (!state.m_is_prepared)
            {
                state.m_context.prepare();
                state.m_is_prepared = true;
            }
            state.m_context.execute();
        }
    }

    node::pin_descriptors run_script_node::get_pin_descriptors() const
    {
        m_pin_descriptors.clear();
        m_variables_to_update.clear();
        m_pin_descriptors.push_back(in_enable_pin::get("enable"));

        if (m_script.is_loaded())
        {       
            int index = static_cast<int>(m_pin_descriptors.size());
            gtl::span<script_descriptor::variable const> variables = m_script->get_descriptor().m_variables;
            for (std::size_t i = 0; i < variables.size(); ++i)
            {
                if (variables[i].m_is_input)
                {
                    m_pin_descriptors.push_back
                    (
                        pin_descriptor{
                            variables[i].m_name.c_str(),
                            index,
                            static_cast<int>(std::hash<gtl::uuid>{}(variables[i].m_id)),
                            variables[i].m_data.get_type_id(),
                            pin_type::input,
                            true,
                        }
                    );
                    m_variables_to_update.emplace_back(
                        variables[i].m_id,
                        index,
                        variables[i].m_is_constant,
                        false
                    );

                    index++;
                }
            }
        }

        return node::pin_descriptors{ m_pin_descriptors, gtl::span<pin_descriptor const> {} };
    }

    void run_script_node::process(gserializer::serializer& serializer)
    {
        node::process(serializer);
        serializer.process("script", m_script);
    }
    node_data run_script_node::get_default_value(int index) const
    {
        if (index == in_enable_pin::index)
        {
            return node_data(true);
        }

        return node::get_default_value(index);
    }
}

