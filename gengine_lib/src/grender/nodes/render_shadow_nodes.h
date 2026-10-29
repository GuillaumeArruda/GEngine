#pragma once

#include "gcore/script/node.h"
#include "gcore/script/script_context.h"

#include "grender/resources/program.h"
#include "grender/resources/frame_buffer_resource.h"
#include "grender/resources/runtime_texture.h"

namespace grender
{
    struct render_shadow_node_base : gcore::node
    {
        using out_texture_pin = gcore::output_pin_descriptor<gcore::resource_handle<texture>, 0, 100>;
        bool is_const() const override { return false; }
        bool is_pure() const override { return false; }

        void prepare(gcore::node_context & context) const override;

    protected:
        struct state
        {
            gcore::resource_handle<runtime_texture> m_texture;
            gcore::resource_handle<frame_buffer_resource> m_framebuffer;
        };
    };

    struct render_directional_shadow_node : render_shadow_node_base
    {
        GCORE_DECLARE_NODE_TYPE(render_directional_shadow_node);

        using in_light_direction_pin = gcore::input_pin_descriptor<glm::vec3, 0, 2>;
        using in_camera_matrix_pin = gcore::input_pin_descriptor<glm::mat4, 1, 3>;
        using in_camera_matrix_projection_pin = gcore::input_pin_descriptor<glm::mat4, 2, 4>;

        using out_light_matrix_pin = gcore::output_pin_descriptor<glm::mat4, 1, 101>;

        void execute(gcore::node_context& context) const override;
        node::pin_descriptors get_pin_descriptors() const override;
    protected:
        static std::array<glm::vec4, 8> get_camera_frustrum_corners(glm::mat4 const& projection, glm::mat4 const& view);
        static glm::mat4 create_light_matrix(glm::vec3 light_direction, std::array<glm::vec4, 8> const& corners);
    };

    struct render_point_light_shadow_node : render_shadow_node_base
    {
        GCORE_DECLARE_NODE_TYPE(render_point_light_shadow_node);
        using in_light_matrix_pin = gcore::input_pin_descriptor<glm::mat4, 0, 2>;
        using in_radius_pin = gcore::input_pin_descriptor<float, 1, 3>;

        void execute(gcore::node_context& context) const override;
        node::pin_descriptors get_pin_descriptors() const override;
    };

    struct render_spot_light_shadow_node : render_shadow_node_base
    {
        GCORE_DECLARE_NODE_TYPE(render_spot_light_shadow_node);
        using in_light_matrix_pin = gcore::input_pin_descriptor<glm::mat4, 0, 2>;
        using in_angle_pin = gcore::input_pin_descriptor<float, 1, 3>;
        using in_distance_pin = gcore::input_pin_descriptor<float, 2, 4>;

        using out_matrix_pin = gcore::output_pin_descriptor<glm::mat4, 1, 101>;

        void execute(gcore::node_context& context) const override;
        node::pin_descriptors get_pin_descriptors() const override;
    };
}