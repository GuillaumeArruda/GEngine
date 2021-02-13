#include "stdafx.h"
#include "node_factory.h"

namespace gcore
{
    node_factory& gcore::node_factory::get()
    {
        static node_factory m_instance;
        return m_instance;
    }

    std::unique_ptr<node> node_factory::try_get_conversion(node_data_type::id_type from, node_data_type::id_type to) const
    {
        auto it = std::find_if(m_conversions.begin(), m_conversions.end(), [&](conversion_info const& conversion) { return conversion.m_from_id == from && conversion.m_to_id == to; });
        if (it != m_conversions.end())
        {
            return create(it->m_node_type_name);
        }
        return std::unique_ptr<node>();
    }

    std::string const& node_factory::get_display_name(node const& node) const
    {
        static std::string const empty_string;
        auto const it = m_type_id_hash_to_display_name.find(std::type_index(typeid(node)));
        if (it != m_type_id_hash_to_display_name.end())
        {
            return it->second;
        }
        return empty_string;
    }
}
