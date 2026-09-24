#pragma once

#include "gcore/script/node.h"
#include "gcore/script/script_context.h"
#include "gcore/resource_library.h"

namespace gcore
{
    template<class ResourceType>
    struct get_resource_node : public node
    {
        GCORE_DECLARE_NODE_TYPE(get_resource_node);

        bool is_const() const override { return false; }
        bool is_pure() const override { return true; }

        using out_pin = output_pin_descriptor<resource_handle<ResourceType>, 0, 1, false>;
        using in_pin = input_pin_descriptor<gtl::uuid, 1, 2, true>;

        void execute(node_context& context) const override
        {
            if (gcore::resource_library* library = context.get_in_context<gcore::resource_library>())
            {
                gtl::span<const gtl::uuid> inputs = context.read<in_pin>();
                gtl::span<gcore::resource_handle<ResourceType>> outputs = context.create_array<out_pin>(inputs.size());
                for (std::size_t i = 0; i < inputs.size(); ++i)
                {
                    outputs[i] = library->template get_resource<ResourceType>(inputs[i]);
                }
            }
        }

        node::pin_descriptors get_pin_descriptors() const override
        {
            static pin_descriptor const outputs[] = {
                out_pin::get("resource"),
            };
            static pin_descriptor const inputs[] = {
                in_pin::get("uuid")
            };

            return { inputs, outputs};
        }
    };
}