#pragma once

#include <unordered_map>

#include "gcore/script/node.h"
#include "gcore/script/nodes/comparison_nodes.h"
#include "gcore/script/node_data_type.h"


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

        template<class From, class To, class NodeType = gcore::conversion_node<From,To>>
        void register_conversion(std::string_view serialization_name)
        {
            node_data_type::id_type const from_id = node_data_type_registry::get_type_id<From>();
            node_data_type::id_type const to_id = node_data_type_registry::get_type_id<To>();
            std::string const display_name = node_data_type_registry::get().get_node_data_type(from_id).get_name() + " to " + node_data_type_registry::get().get_node_data_type(to_id).get_name();
            register_type<NodeType>(serialization_name, display_name);
            m_conversions.push_back(conversion_info{from_id, to_id, std::string(serialization_name)});
        }

        std::unique_ptr<node> try_get_conversion(node_data_type::id_type from, node_data_type::id_type to) const;

        std::string const& get_display_name(node const& node) const;
    private:
        struct conversion_info
        {
            node_data_type::id_type m_from_id;
            node_data_type::id_type m_to_id;
            std::string m_node_type_name;
        };
        std::unordered_map<std::type_index, std::string> m_type_id_hash_to_display_name;
        std::vector<conversion_info> m_conversions;
        node_factory() = default;
    };
}
