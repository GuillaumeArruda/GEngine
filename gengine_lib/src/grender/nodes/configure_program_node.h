#pragma once

#include "gcore/script/node.h"
#include "gcore/script/script_context.h"

#include "grender/resources/program.h"

namespace grender
{
    struct configure_program_node : gcore::node
    {
        GCORE_DECLARE_NODE_TYPE(configure_program_node);
        using out_pin = gcore::output_pin_descriptor<program_pin_data, 0, 12000>;

        bool is_const() const override { return false; }
        bool is_pure() const override { return false; }

        void execute(gcore::node_context& context) const override;

        void process(gserializer::serializer& process) override;

        node::pin_descriptors get_pin_descriptors() const override;

    private:
        void set_state_at_index(gcore::node_context& context, program_uniform_state& state, std::size_t index) const;
        gcore::node_data_type::id_type get_variant_type_id(program_uniform_state const& state, std::size_t index) const;

        gcore::resource_handle<program> m_program;
        mutable std::vector<gcore::pin_descriptor> m_descriptors;
    };
}