#pragma once

#include "gcore/script/node.h"
#include "gcore/script/script.h"
#include "gcore/script/script_context.h"

namespace gcore
{
    struct run_script_node : node
    {
        GCORE_DECLARE_NODE_TYPE(run_script_node);

        using in_enable_pin = input_pin_descriptor<bool, 0>;

        bool is_const() const override { return false; }
        bool is_pure() const override { return false; }
        bool is_root() const override { return true; }

        void prepare(node_context& context) const override;
        void execute(node_context& context) const override;

        node::pin_descriptors get_pin_descriptors() const override;
        void process(gserializer::serializer& serializer) override;
        node_data get_default_value(int index) const override;

    protected:
        struct variable_update
        {
            gtl::uuid m_id;
            std::size_t m_index = 0;
            bool m_update_once = false;
            bool m_has_updated = false;
        };
        
        struct state
        {
            gcore::script_context m_context;
            std::vector<variable_update> m_variables_to_update;
            bool m_is_prepared = false;
        };

        mutable std::vector<pin_descriptor> m_pin_descriptors;
        mutable std::vector<variable_update> m_variables_to_update;
        gcore::resource_handle<script> m_script;
    };
}