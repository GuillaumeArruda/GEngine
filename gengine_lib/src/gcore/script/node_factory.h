#pragma once

#include <unordered_map>

#include "gcore/script/node.h"

#include "gserializer/type_factory.h"

namespace gcore
{
    struct node_factory
        : gserializer::type_factory<node>
    {
        static node_factory& get();

        template<class NodeType>
        void register_type(std::string_view serialization_name, std::string_view display_name)
        {
            gserializer::type_factory<node>::register_type<NodeType>(serialization_name);
            m_type_id_hash_to_display_name[std::type_index(typeid(NodeType))] = display_name;
        }

        std::string const& get_display_name(node const& node) const;

    private:
        std::unordered_map<std::type_index, std::string> m_type_id_hash_to_display_name; 
        node_factory() = default;
    };
}
