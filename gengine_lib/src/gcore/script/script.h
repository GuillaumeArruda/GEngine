#pragma once

#include <vector>
#include <memory>
#include <glm/vec2.hpp>

#include "gcore/resource.h"

#include "gcore/script/node.h"

namespace gserializer
{
    struct serializer;
}

namespace gcore
{
    struct script_context;

    struct script_descriptor
    {
        struct connection
        {
            connection() = default;
            connection(node_id_t source_id, int source_pin, int destination_pin) : m_source_id(source_id), m_source_pin_id(source_pin), m_destination_pin_id(destination_pin) {}
            node_id_t m_source_id = 0;
            int m_source_pin_id = 0;
            int m_destination_pin_id = 0;
            void process(gserializer::serializer& serializer);
        };

        struct node_descriptor
        {
            std::unique_ptr<node> m_node;
            glm::vec2 m_position;
            std::string m_name;
            void process(gserializer::serializer& serializer);
        };

        pin_descriptor const* get_pin_descriptor(gcore::node_id_t node_id, int pin_id) const;
        pin_descriptor* get_pin_descriptor(gcore::node_id_t node_id, int pin_id);
        node_descriptor const* get_node_descriptor(gcore::node_id_t node_id) const;
        node_descriptor* get_node_descriptor(gcore::node_id_t node_id);

        bool can_add_connection(node_id_t source, int source_pin_id, node_id_t destination, int destination_pin_id) const;
        bool add_connection(node_id_t source, int source_pin_id, node_id_t destination, int destination_pin_id, node*& out_conversion_node);
        void remove_connection(node_id_t destination, int destination_pin_id);

        std::vector<node_descriptor> m_nodes;
        std::unordered_map<node_id_t, std::vector<connection>> m_connections;
        gcore::node_id_t m_node_id_generator = 1; 
        void process(gserializer::serializer& serializer);
    };


    struct script : resource
    {
        using super = resource;
        bool do_load_async() override;
        bool do_load_sync() override { return true; }
        void do_unload() override;

        void process(gserializer::serializer& serializer) override;

        node const* get_node(std::uint32_t node_index) const { return m_nodes[node_index]; }

        std::size_t get_necessary_memory_for_context() const { return m_number_of_bytes_for_context; }

        script_context create_context() const;
        gtl::span<std::uint32_t const> get_root_node_indexes() const { return m_root_node_indexes; }

        GSERIALIZER_DECLARE_SUBCLASS_FACTORY_REGISTRATION();
    private:
        bool are_all_input_const(int node_index) const;

        std::filesystem::path m_descriptor_path;
        std::unique_ptr<char[]> m_node_buffer;
        std::vector<node*> m_nodes;
        std::vector<std::uint32_t> m_root_node_indexes;
        std::unique_ptr<script_context> m_default_script_context;
        std::size_t m_number_of_bytes_for_context = 0;
    };
}