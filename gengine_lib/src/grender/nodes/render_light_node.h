#pragma once

#include "gcore/script/node.h"
#include "gcore/resource_handle.h"
#include "gcore/script/script_context.h"

#include "grender/resources/mesh_resource.h"
#include "grender/resources/program.h"

namespace grender
{
    struct render_light_node : gcore::node
    {
        GCORE_DECLARE_NODE_TYPE(render_light_node);
        bool is_const() const override { return false; }
        bool is_pure() const override { return false; }
        bool is_root() const override { return true; }

        using in_enable_pin = gcore::input_pin_descriptor<bool, 0, 2>;
        using in_transform_pin = gcore::input_pin_descriptor<glm::mat4, 1, 3>;
        using in_program_pin = gcore::input_pin_descriptor<program_pin_data, 2, 1>;
        using in_cull_back_pin = gcore::input_pin_descriptor<bool, 3, 4>;

        void execute(gcore::node_context& context) const override;

        void process(gserializer::serializer& serializer) override;

        node::pin_descriptors get_pin_descriptors() const override;

        gcore::resource_handle<mesh_resource> get_mesh() const { return m_mesh; }

    private:
        gcore::resource_handle<mesh_resource> m_mesh;
    };
}