#include "stdafx.h"
#include "node_factory.h"

namespace gcore
{
    node_factory& gcore::node_factory::get()
    {
        static node_factory m_instance;
        return m_instance;
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
