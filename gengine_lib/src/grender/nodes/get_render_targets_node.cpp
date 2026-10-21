#include "stdafx.h"

#include "grender/nodes/get_render_targets_node.h"
#include "grender/systems/render_system.h"

namespace grender
{
    void get_render_targets_node::execute(gcore::node_context& context) const
    {
        if (auto const* render_sys = context.get_in_context<std::reference_wrapper<render_system>>())
        {
            context.write_values<out_diffuse_tex_pin>(render_sys->get().get_render_target(frame_buffer::render_target_type::diffuse));
            context.write_values<out_position_tex_pin>(render_sys->get().get_render_target(frame_buffer::render_target_type::position));
            context.write_values<out_normal_tex_matrix_pin>(render_sys->get().get_render_target(frame_buffer::render_target_type::normal));
            context.write_values<out_specular_tex_matrix_pin>(render_sys->get().get_render_target(frame_buffer::render_target_type::specular));
            context.write_values<out_texture_size_pin>(render_sys->get().get_target_size());
        }
    }

    gcore::node::pin_descriptors get_render_targets_node::get_pin_descriptors() const
    {
        static gcore::pin_descriptor const outputs[] = {
            out_diffuse_tex_pin::get("diffuse texture"),
            out_position_tex_pin::get("position texture"),
            out_normal_tex_matrix_pin::get("normal texture"),
            out_specular_tex_matrix_pin::get("specular texture"),
            out_texture_size_pin::get("texture size"),
        };
        return  { gtl::span<gcore::pin_descriptor const>{}, outputs };
    }
}

