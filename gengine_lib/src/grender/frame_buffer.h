#pragma once

#include <array>

#include "gtl/utility.h"

#include "grender/render_target.h"


namespace grender
{
    struct frame_buffer
    {
        enum class render_target_type
        {
            diffuse,
            position,
            normal,
            specular,
            final_color,
            depth,
            count
        };

        frame_buffer(GLsizei width, GLsizei height);
        ~frame_buffer();

        frame_buffer(frame_buffer&& move) noexcept;
        frame_buffer& operator=(frame_buffer&& move) noexcept;

        frame_buffer(frame_buffer const&) = delete;
        frame_buffer& operator=(frame_buffer const&) = delete;

        GLuint get_render_target_id(render_target_type type) const { return m_render_targets[gtl::to_underlying(type)].get_id(); }

        void bind_for_geometry() const;
        void bind_for_stencil() const;
        void bind_for_light() const;
        void transfer_to_default() const;
        void unbind() const;
    private:
        std::array<render_target, gtl::to_underlying(render_target_type::count)> m_render_targets;
        GLsizei m_width = 0;
        GLsizei m_height = 0;
        GLuint m_fbo = 0;
    };
}