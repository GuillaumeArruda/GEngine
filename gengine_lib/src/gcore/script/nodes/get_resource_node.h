#pragma once

#include "gcore/script/node.h"
#include "gcore/script/script_context.h"

namespace gcore
{
    template<class ResourceType>
    struct get_resource_node : public node
    {
        GCORE_DECLARE_NODE_TYPE(get_resource_node);

        bool is_const() const override { return true; }

        using out_pin = output_pin_descriptor<resource_handle<ResourceType>, 0, 1, false>;

        void execute(node_context& context) const override
        {
            context.write_values<out_pin>(m_handle);
        }

        void process(gserializer::serializer& serializer)
        {
            node::process(serializer);
            serializer.process("resource_uuid", m_handle);
        }

        node::pin_descriptors get_pin_descriptors() const override
        {
            static pin_descriptor const outputs[] = {
                out_pin::get("resource"),
            };
            return {gtl::span<pin_descriptor const>{}, outputs};
        }

    private:
        resource_handle<ResourceType> m_handle;
    };
}