
#include "stdafx.h"

#include "gcore/script/node_data_type.h"

namespace gcore
{
    node_data_type_registry node_data_type_registry::m_instance;

    node_data_type const* node_data_type_registry::get_by_name(std::string_view name) const
    {
        auto const it = std::find_if(m_node_data_types.begin(), m_node_data_types.end(), [&](node_data_type const& data_type) { return data_type.get_name() == name; });
        if (it != m_node_data_types.end())
            return &(*it);
        return nullptr;
    }
}