#pragma once

#include "gcore/script/node.h"
#include "gcore/script/script_context.h"

#include "grender/resources/program.h"
#include "grender/resources/frame_buffer_resource.h"
#include "grender/resources/runtime_texture.h"

namespace grender
{
    struct render_directional_shadow_node : gcore::node
    {
        GCORE_DECLARE_NODE_TYPE(render_directional_shadow_node);

        using in_program_pin = gcore::input_pin_descriptor<program_pin_data, 0, 1>;
        using in_light_direction_pin = gcore::input_pin_descriptor<glm::vec3, 1, 2>;
        using in_camera_matrix_pin = gcore::input_pin_descriptor<glm::mat4, 2, 3>;
        using in_camera_matrix_projection_pin = gcore::input_pin_descriptor<glm::mat4, 3, 4>;

        using out_texture_pin = gcore::output_pin_descriptor<gcore::resource_handle<texture>, 0, 100>;
        using out_light_matrix_pin = gcore::output_pin_descriptor<glm::mat4, 1, 101>;

        bool is_const() const override { return false; }
        bool is_pure() const override { return false; }

        void prepare(gcore::node_context& context) const override;
        void execute(gcore::node_context& context) const override;
        node::pin_descriptors get_pin_descriptors() const override;
    
    protected:
        struct state
        {
            gcore::resource_handle<runtime_texture> m_texture;
            gcore::resource_handle<frame_buffer_resource> m_framebuffer;
        };

        static std::array<glm::vec4, 8> get_camera_frustrum_corners(glm::mat4 const& projection, glm::mat4 const& view);
        static glm::mat4 create_light_matrix(glm::vec3 light_direction, std::array<glm::vec4, 8> const& corners);
    };

    struct render_point_light_shadow_node : gcore::node
    {
        GCORE_DECLARE_NODE_TYPE(render_point_light_shadow_node);

        using in_program_pin = gcore::input_pin_descriptor<program_pin_data, 0, 1>;
        using in_light_matrix_pin = gcore::input_pin_descriptor<glm::mat4, 1, 2>;
        using in_radius_pin = gcore::input_pin_descriptor<float, 2, 3>;

        using out_texture_pin = gcore::output_pin_descriptor<gcore::resource_handle<texture>, 0, 100>;

        bool is_const() const override { return false; }
        bool is_pure() const override { return false; }

        void prepare(gcore::node_context& context) const override;
        void execute(gcore::node_context& context) const override;
        node::pin_descriptors get_pin_descriptors() const override;

    protected:
        struct state
        {
            gcore::resource_handle<runtime_texture> m_texture;
            gcore::resource_handle<frame_buffer_resource> m_framebuffer;
        };
    };
}