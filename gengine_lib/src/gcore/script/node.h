#pragma once

#include <array>
#include <cassert>

#include "gtl/span.h"

#include "gcore/script/node_data_type.h"
#include "gcore/script/script_defines.h"

#include "gtl/uuid.h"
#include "gtl/utility.h"

#include "gserializer/type_factory.h"

namespace gserializer
{
    struct serializer;
}

#define GCORE_DECLARE_NODE_TYPE(NodeType)                                                                   \
    NodeType* clone(void* location) const override { return new(location) NodeType(*this); }                \
    std::size_t byte_size() const { return sizeof(*this); } 


namespace gcore
{
    struct node_context;
    struct pin_descriptor;
    struct alignas(16) node
    {
        virtual ~node() = default;

        virtual void prepare(node_context&) {};
        virtual void execute(node_context& context) const = 0;

        virtual node* clone(void* location) const = 0;
        virtual std::size_t byte_size() const = 0;
        
        virtual bool is_const() const { return true; } // const node will only be called once to setup their output value
        virtual bool is_pure() const { return is_const(); } // pure node will propagate the constness of their input. 
        virtual bool is_root() const { return false; }  // If true, the node will be executed when the script start

        using pin_descriptors = std::array<gtl::span<pin_descriptor const>, gtl::to_underlying(pin_type::count)>;

        virtual pin_descriptors get_pin_descriptors() const = 0;

        virtual void process(gserializer::serializer& serializer);

        node_id_t get_node_id() const { return m_node_id; }
        void set_node_id(node_id_t id) { assert(m_node_id == 0); m_node_id = id; }

        virtual void on_input_pin_state_changed(int /*pin_id*/, pin_state /*pin_state*/) {};

    private:
        node_id_t m_node_id = 0;
    };
}