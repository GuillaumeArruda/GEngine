#pragma once

#include "script.h"

namespace gserializer
{
    struct serializer;
}

namespace gtool
{
    template<class ScriptType>
    void display_script_runner(struct gtool::imgui_serializer&, void*);
}

namespace gcore
{
    struct script_runner_base
    {
        struct input
        {
            gtl::uuid m_variable_id;
            node_data m_value;
            void process(gserializer::serializer& serializer);
        };
    };

    template<class ScriptType>
    struct script_runner : script_runner_base
    {
        void process(gserializer::serializer& serializer)
        {
            serializer.process("script", m_script);
            serializer.process("inputs", m_inputs, "input");
        }

        gcore::resource_handle<ScriptType> const& get_script() const { return m_script; }
        bool is_loaded() const noexcept { return m_script.is_loaded(); }
        bool is_ready() const noexcept { return is_loaded() && m_context.is_prepared(); }
        void execute() { m_context.execute(); }
        template<class ... ContextInput>
        void prepare(ContextInput&&... input)
        {
            m_context = m_script->create_context();
            (m_context.set_in_context(std::forward<ContextInput>(input)), ... );
            for (auto& in : m_inputs)
            {
                if (node_data* data = m_context.get_variable(in.m_variable_id))
                {
                    if (data->get_type_id() == in.m_value.get_type_id())
                    {
                        (*data) = in.m_value;
                    }
                }
            }
            m_context.prepare();
        }

        template<class ContextElement>
        void set_in_context(ContextElement&& element)
        {
            m_context.set_in_context(std::forward<ContextElement>(element));
        }

    protected:
        friend void gtool::display_script_runner<ScriptType>(gtool::imgui_serializer&, void*);
        script_context m_context;
        std::vector<input> m_inputs;
        gcore::resource_handle<ScriptType> m_script;
    };
}
