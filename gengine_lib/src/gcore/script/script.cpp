#include "stdafx.h"

#include "gcore/script/script.h"
#include "gcore/script/script_context.h"
#include "gcore/script/nodes/constant_node.h"
#include "gcore/script/node_factory.h"

#include "gcore/script/nodes/comparison_nodes.h"

#include "gserializer/serializer.h"

#include "gserializer/serializers/json_serializer.h"
#include "gserializer/gmath_serialization.h"

GSERIALIZER_DEFINE_SUBCLASS_FACTORY_REGISTRATION(gcore::script);

namespace gcore
{
    pin_descriptor const* script_descriptor::get_pin_descriptor(gcore::node_id_t node_id, int pin_id) const
    {
        node_descriptor const* node_desc = get_node_descriptor(node_id);
        if (!node_desc)
            return nullptr;

        node::pin_descriptors const descriptors = node_desc->m_node->get_pin_descriptors();

        for (gtl::span<pin_descriptor const> pin_descriptors : descriptors)
        {
            auto const it = std::find_if(pin_descriptors.begin(), pin_descriptors.end(), [&](pin_descriptor const& desc) { return desc.m_id == pin_id; });
            if (it != pin_descriptors.end())
            {
                return &(*it);
            }
        }

        return nullptr;
    }

    pin_descriptor* script_descriptor::get_pin_descriptor(node_id_t node_id, int pin_id)
    {
        return const_cast<pin_descriptor*>(static_cast<script_descriptor const*>(this)->get_pin_descriptor(node_id, pin_id));
    }

    script_descriptor::node_descriptor const* script_descriptor::get_node_descriptor(gcore::node_id_t node_id) const
    {
        auto const node_it = std::find_if(m_nodes.begin(), m_nodes.end(), [&](node_descriptor const& node_desc) { return node_desc.m_node->get_node_id() == node_id; });
        if (node_it != m_nodes.end())
            return &(*node_it);

        return nullptr;
    }

    script_descriptor::node_descriptor* script_descriptor::get_node_descriptor(gcore::node_id_t node_id)
    {
        return const_cast<node_descriptor*>(static_cast<script_descriptor const*>(this)->get_node_descriptor(node_id));
    }

    bool script_descriptor::can_add_connection(node_id_t source, int source_pin_id, node_id_t destination, int destination_pin_id) const
    {
        gcore::pin_descriptor const* source_pin_desc = get_pin_descriptor(source, source_pin_id);
        gcore::pin_descriptor const* destination_pin_desc = get_pin_descriptor(destination, destination_pin_id);
        return source_pin_desc && destination_pin_desc && (source_pin_desc->m_type_id == destination_pin_desc->m_type_id || gcore::node_factory::get().has_conversion(source_pin_desc->m_type_id, destination_pin_desc->m_type_id)) && source_pin_desc->m_pin_type == gcore::pin_type::output && destination_pin_desc->m_pin_type == gcore::pin_type::input;
    }

    bool script_descriptor::add_connection(node_id_t source, int source_pin_id, node_id_t destination, int destination_pin_id, node*& out_conversion_node)
    {
        if (can_add_connection(source, source_pin_id, destination, destination_pin_id))
        {
            gcore::pin_descriptor const* source_pin_desc = get_pin_descriptor(source, source_pin_id);
            gcore::pin_descriptor const* destination_pin_desc = get_pin_descriptor(destination, destination_pin_id);

            if (std::unique_ptr<node> conversion_node = gcore::node_factory::get().try_get_conversion(source_pin_desc->m_type_id, destination_pin_desc->m_type_id))
            {
                conversion_node->set_node_id(m_node_id_generator++);
                auto& connections = m_connections[conversion_node->get_node_id()];
                gcore::node::pin_descriptors const pins = conversion_node->get_pin_descriptors();

                assert(pins[gtl::to_underlying(gcore::pin_type::output)].size() == 1);
                assert(pins[gtl::to_underlying(gcore::pin_type::input)].size() == 1);

                connections.emplace_back(source, source_pin_id, pins[gtl::to_underlying(gcore::pin_type::input)][0].m_id);
                source = conversion_node->get_node_id();
                source_pin_id = pins[gtl::to_underlying(gcore::pin_type::output)][0].m_id;

                out_conversion_node = conversion_node.get();
                conversion_node->on_input_pin_state_changed(pins[gtl::to_underlying(gcore::pin_type::input)][0].m_id, pin_state::connected);
                gcore::script_descriptor::node_descriptor node_desc;
                node_desc.m_node = std::move(conversion_node);
                m_nodes.push_back(std::move(node_desc));
            }

            auto& connections = m_connections[destination];
            auto const it = std::find_if(connections.begin(), connections.end(), [&](gcore::script_descriptor::connection const& connection) { return connection.m_destination_pin_id == destination_pin_id; });
            if (it != connections.end())
                connections.erase(it);

            connections.emplace_back(source, source_pin_id, destination_pin_id);
            get_node_descriptor(destination)->m_node->on_input_pin_state_changed(destination_pin_id, pin_state::connected);
            return true;
        }
        return false;
    }

    void script_descriptor::remove_connection(node_id_t destination, int destination_pin_id)
    {
        auto& connections = m_connections[destination];
        auto const it = std::find_if(connections.begin(), connections.end(), [&](connection const& connection) { return connection.m_destination_pin_id == destination_pin_id; });
        if (it != connections.end())
        {
            connections.erase(it);
            get_node_descriptor(destination)->m_node->on_input_pin_state_changed(destination_pin_id, pin_state::disconnected);
        }
    }

    void script_descriptor::process(gserializer::serializer& serializer)
    {
        serializer.process("nodes", m_nodes, "node");
        serializer.process("connections", m_connections, "node_connection", "connection");
    }


    void script_descriptor::node_descriptor::process(gserializer::serializer& serializer)
    {
        serializer.process("name", m_name);
        serializer.process("position", m_position);
        serializer.process("node", m_node, node_factory::get());
    }

    void script_descriptor::connection::process(gserializer::serializer& serializer)
    {
        serializer.process("source", m_source_id);
        serializer.process("source_pin", m_source_pin_id);
        serializer.process("destination_pin", m_destination_pin_id);
    }

    bool script::do_load_async()
    {
        gserializer::json_read_serializer serializer(m_descriptor_path.string().c_str());
        script_descriptor descriptor;
        descriptor.process(serializer);

        std::size_t node_memory_to_allocate = 0;
        std::size_t number_of_input_to_allocate = 0;
        std::size_t number_of_output_to_allocate = 0;
        std::vector<std::unique_ptr<constant_node>> new_constant_node;

        gcore::node_id_t id_generator = 0;
        std::unordered_map<node_id_t, int> node_id_to_node_index;
        for (std::size_t i = 0; i < descriptor.m_nodes.size(); ++i)
        {
            id_generator = std::max(id_generator, descriptor.m_nodes[i].m_node->get_node_id());
            auto success = node_id_to_node_index.insert(std::pair(descriptor.m_nodes[i].m_node->get_node_id(), static_cast<int>(i)));
            assert(success.second);
        }

        id_generator++; // All node id are guaranteed to be smaller than this

        //Compute how much memory is necessary to allocate for the graph and context
        for (auto const& node : descriptor.m_nodes)
        {
            node_memory_to_allocate += node.m_node->byte_size();

            node::pin_descriptors const pins = node.m_node->get_pin_descriptors();
            
            number_of_input_to_allocate += pins[gtl::to_underlying(pin_type::input)].size();
            number_of_output_to_allocate += pins[gtl::to_underlying(pin_type::output)].size();
            
            //Make sure all inputs are connected to a node, if not create a constant node and add a connection
            std::vector<script_descriptor::connection>& node_connections = descriptor.m_connections[node.m_node->get_node_id()];
            for (gcore::pin_descriptor const& input_descriptor : pins[gtl::to_underlying(pin_type::input)])
            {
                auto const it = std::find_if(node_connections.begin(), node_connections.end(), [&](script_descriptor::connection const& connection) { return connection.m_destination_pin_id == input_descriptor.m_id; });
                if (it == node_connections.end() && !input_descriptor.m_is_optional)
                {
                    new_constant_node.push_back(std::make_unique<constant_node>());
                    std::unique_ptr<constant_node>& constant = new_constant_node.back();
                    constant->set_node_id(id_generator++);
                    constant->get_node_data().set_type_id(input_descriptor.m_type_id);
                    auto success = node_id_to_node_index.insert(std::pair(constant->get_node_id(), static_cast<int>(descriptor.m_nodes.size() + new_constant_node.size()) - 1));
                    node_memory_to_allocate += constant->byte_size();
                    assert(success.second);

                    number_of_output_to_allocate++;

                    script_descriptor::connection new_connection{constant->get_node_id(), constant_node::output_id, input_descriptor.m_id};
                    node_connections.push_back(new_connection);
                }
            }
        }


        //Create final node location
        {
            m_node_buffer = std::make_unique<char[]>(node_memory_to_allocate);
            char* current_location = m_node_buffer.get();
            m_nodes.reserve(descriptor.m_nodes.size());
            for (auto const& node : descriptor.m_nodes)
            {
                m_nodes.push_back(node.m_node->clone(static_cast<void*>(current_location)));
                current_location += node.m_node->byte_size();
                if (m_nodes.back()->is_root())
                {
                    m_root_node_indexes.push_back(static_cast<std::uint32_t>(m_nodes.size() - 1));
                }
            }
            for (auto const& node : new_constant_node)
            {
                m_nodes.push_back(node->clone(static_cast<void*>(current_location)));
                current_location += node->byte_size();
            }
        }

        // Create input output structure of the default context
        {
            m_number_of_bytes_for_context = number_of_input_to_allocate * sizeof(in_pin_data) + number_of_output_to_allocate * sizeof(node_data);
            m_default_script_context = std::make_unique<script_context>();
            m_default_script_context->m_script = this;
            m_default_script_context->m_memory_buffer = std::make_unique<char[]>(m_number_of_bytes_for_context);
            char* current_location = m_default_script_context->m_memory_buffer.get();
            for (auto const& node : m_nodes)
            {
                node::pin_descriptors const pins = node->get_pin_descriptors();
                char* const input_beginning = current_location;
                for (std::size_t i = 0; i < pins[gtl::to_underlying(pin_type::input)].size(); ++i)
                {
                    new(current_location) in_pin_data();
                    reinterpret_cast<in_pin_data*>(current_location)->m_is_optional = pins[gtl::to_underlying(pin_type::input)][i].m_is_optional;
                    current_location += sizeof(in_pin_data);
                }

                char* const output_beginning = current_location;
                for (std::size_t i = 0; i < pins[gtl::to_underlying(pin_type::output)].size(); ++i)
                {
                    new(current_location) node_data();
                    current_location += sizeof(node_data);
                }

                m_default_script_context->m_node_contexts.emplace_back(*m_default_script_context, gtl::span{ reinterpret_cast<in_pin_data*>(input_beginning), pins[gtl::to_underlying(pin_type::input)].size() }, gtl::span{ reinterpret_cast<node_data*>(output_beginning), pins[gtl::to_underlying(pin_type::output)].size() });
            }
        }

        // Connect input to output
        {
            for (std::size_t i = 0; i < m_nodes.size(); ++i)
            {
                node const& input_node = *m_nodes[i];
                gtl::span<pin_descriptor const> input_pin_descriptors = input_node.get_pin_descriptors()[gtl::to_underlying(pin_type::input)];
                node_context& input_context = m_default_script_context->m_node_contexts[i];
                auto const it = descriptor.m_connections.find(input_node.get_node_id());
                if (it == descriptor.m_connections.end())
                {
                    continue;
                }
                std::vector<script_descriptor::connection> const& input_node_connections = it->second;

                for (script_descriptor::connection const& connection : input_node_connections)
                {
                    int const in_pin_index = std::find(input_pin_descriptors.begin(), input_pin_descriptors.end(), connection.m_destination_pin_id)->m_index;
                    in_pin_data& input_data = input_context.m_input_data[in_pin_index];

                    int const output_index = node_id_to_node_index.find(connection.m_source_id)->second;
                    node const& output_node = *m_nodes[output_index];
                    node_context const& output_context = m_default_script_context->m_node_contexts[output_index];
                    gtl::span<pin_descriptor const> output_pin_descriptors = output_node.get_pin_descriptors()[gtl::to_underlying(pin_type::output)];
                    int const output_pin_index = std::find(output_pin_descriptors.begin(), output_pin_descriptors.end(), connection.m_source_pin_id)->m_index;
                    
                    input_data.m_node_index = output_index;
                    auto const output_location = reinterpret_cast<std::ptrdiff_t>(output_context.m_output_data.begin() + output_pin_index);
                    auto const input_location = reinterpret_cast<std::ptrdiff_t>(&input_data);
                    input_data.m_diff_to_node_data = output_location - input_location;
                }
            }
        }

        // Set is constant flag on input pin
        {
            for (std::size_t i = 0; i < m_nodes.size(); ++i)
            {
                node_context& input_context = m_default_script_context->m_node_contexts[i];
                for (in_pin_data& in_data : input_context.m_input_data)
                {
                    in_data.m_is_constant = are_all_input_const(in_data.m_node_index);
                }
            }
        }

        return true;
    }

    void script::do_unload()
    {
        m_default_script_context.reset();
        for (node* node : m_nodes)
            node->~node();
        m_node_buffer.reset();
        m_number_of_bytes_for_context = 0;
    }

    void script::process(gserializer::serializer& serializer)
    {
        super::process(serializer);
        serializer.process("descriptor_file", m_descriptor_path);
    }

    bool script::are_all_input_const(int node_index) const
    {
        if (m_nodes[node_index]->is_const())
        {
            return true;
        }
        else if (m_nodes[node_index]->is_pure())
        {
            bool are_all_const = true;
            for (in_pin_data const& data : m_default_script_context->m_node_contexts[node_index].m_input_data)
            {
                are_all_const &= are_all_input_const(data.m_node_index);
            }
            return are_all_const;
        }
        return false;
    }

    script_context script::create_context() const
    {
        return script_context(*m_default_script_context);
    }
}

