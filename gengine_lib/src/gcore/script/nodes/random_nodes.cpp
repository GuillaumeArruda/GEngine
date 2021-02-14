#include "stdafx.h"
#include "random_nodes.h"


namespace gcore
{
    void random_integer_range_node::execute_on_element(node_context&, int const& min, int const& max, int& value) const
    {
        if (max <= 0)
            value = 0;
        else
            value = (std::rand() % max) + min;
    }

    node::pin_descriptors random_integer_range_node::get_pin_descriptors() const
    {
        static pin_descriptor const input[] = { super::in_pin_1::get("min"), super::in_pin_2::get("max") };
        static pin_descriptor const output[] = { super::out_pin::get("result") };
        return { input, output };
    }

}
