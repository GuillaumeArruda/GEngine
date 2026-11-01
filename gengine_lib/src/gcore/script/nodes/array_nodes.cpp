#include <stdafx.h>

#include "gcore/script/nodes/array_nodes.h"

namespace gcore
{
   void gcore::array_size_node::execute(node_context& context) const
   {
       if (node_data const* input = context.get_input_node_data(0))
       {
           context.write_values<output>(static_cast<int>(input->get_number_of_elements()));
       }
       else
       {
           context.write_values<output>(0);
       }
   }

    node::pin_descriptors array_size_node::get_pin_descriptors() const
    {
        static std::array const inputs = {
            pin_descriptor{ .m_name = "input", .m_type_id = node_data_type::all_id, .m_is_optional = true }
        };

        static std::array const outputs = {
            output::get("size")
        };

        return node::pin_descriptors{ inputs, outputs };
    }
}

