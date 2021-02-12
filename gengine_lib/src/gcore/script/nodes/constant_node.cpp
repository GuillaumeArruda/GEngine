#include "stdafx.h"
#include "gcore/script/nodes/constant_node.h"


namespace gcore
{
    void constant_node::execute(node_context& context) const
    {
        context.copy_data_to_output(m_data, output_index);
    }

    node::pin_descriptors constant_node::get_pin_descriptors() const
    {
        m_pin_descriptor = pin_descriptor{ "value",output_index, output_id, m_data.get_type_id(), pin_type::output };
        return { gtl::span<pin_descriptor const>(), gtl::span<pin_descriptor const>(m_pin_descriptor) };
    }
}

