#include "stdafx.h"

#include "grender/frame_buffer.h"

#include "grender/utils.h"

namespace grender
{
    frame_buffer::frame_buffer(GLsizei width, GLsizei height)
        : m_width(width)
        , m_height(height)
    {
        gl_exec(glGenFramebuffers, 1, &m_fbo);
        gl_exec(glBindFramebuffer, GL_DRAW_FRAMEBUFFER, m_fbo);

        m_render_targets[gtl::to_underlying(render_target_type::diffuse)] = render_target(GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE, GL_COLOR_ATTACHMENT0, GL_LINEAR, width, height);                     // DiffuseColor
        m_render_targets[gtl::to_underlying(render_target_type::position)] = render_target(GL_RGB32F, GL_RGB, GL_FLOAT, GL_COLOR_ATTACHMENT1, GL_LINEAR, width, height);                             // Position
        m_render_targets[gtl::to_underlying(render_target_type::normal)] = render_target(GL_RGB32F, GL_RGB, GL_FLOAT, GL_COLOR_ATTACHMENT2, GL_LINEAR, width, height);                             // Normal
        m_render_targets[gtl::to_underlying(render_target_type::specular)] = render_target(GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE, GL_COLOR_ATTACHMENT3, GL_LINEAR, width, height);                     // SpecularColor
        m_render_targets[gtl::to_underlying(render_target_type::final_color)] = render_target(GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, GL_COLOR_ATTACHMENT4, GL_LINEAR, width, height);                     // FinalColor
        m_render_targets[gtl::to_underlying(render_target_type::depth)] = render_target(GL_DEPTH32F_STENCIL8, GL_DEPTH_STENCIL, GL_FLOAT_32_UNSIGNED_INT_24_8_REV, GL_DEPTH_STENCIL_ATTACHMENT, GL_NEAREST, width, height); //Depth

        gl_exec(glFramebufferTexture2D, GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_render_targets[gtl::to_underlying(render_target_type::depth)].get_id(), 0);

        const GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
        glDrawBuffers(4, DrawBuffers);
        GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

        if (status != GL_FRAMEBUFFER_COMPLETE)
        {
            std::cerr << "FrameBuffer is incomplete\n";
        }

        unbind();
    }
    
    frame_buffer::~frame_buffer()
    {
        gl_exec(glDeleteBuffers, 1, &m_fbo);
    }

    frame_buffer::frame_buffer(frame_buffer&& move) noexcept
        : m_fbo(move.m_fbo)
        , m_render_targets(std::move(move.m_render_targets))
    {
    }

    frame_buffer& frame_buffer::operator=(frame_buffer&& move) noexcept
    {
        if (this == &move)
            return *this;

        gl_exec(glDeleteBuffers, 1, &m_fbo);
        m_render_targets = std::move(move.m_render_targets);
        m_fbo = move.m_fbo;
        move.m_fbo = 0;

        return *this;
    }

    void frame_buffer::unbind() const
    {
        gl_exec(glBindFramebuffer, GL_DRAW_FRAMEBUFFER, 0);
    }

    void frame_buffer::bind_for_geometry() const
    {
        gl_exec(glBindFramebuffer, GL_DRAW_FRAMEBUFFER, m_fbo);
        GLenum const draw_buffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
        gl_exec(glDrawBuffers, 4, draw_buffers);
    }

    void frame_buffer::bind_for_stencil() const
    {
        glDrawBuffer(GL_NONE);
    }

    void frame_buffer::bind_for_light() const
    {
        gl_exec(glDrawBuffer, GL_COLOR_ATTACHMENT4);
    }

    void frame_buffer::transfer_to_default() const
    {
        gl_exec(glBindFramebuffer, GL_DRAW_FRAMEBUFFER, 0);
        gl_exec(glBindFramebuffer, GL_READ_FRAMEBUFFER, m_fbo);
        gl_exec(glReadBuffer, GL_COLOR_ATTACHMENT4);
        gl_exec(glBlitFramebuffer, 0, 0, m_width, m_height,
            0, 0, m_width, m_height, GL_COLOR_BUFFER_BIT, GL_LINEAR);
    }
}
