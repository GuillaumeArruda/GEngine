#include "stdafx.h"

#include "grender/render_target.h"

#include "grender/utils.h"

namespace grender
{
    render_target::render_target(GLint internalFormat, GLenum format, GLenum type, GLenum attachment, GLenum filterType, GLsizei width, GLsizei height)
    {
        gl_exec(glGenTextures,1, &m_id);
        gl_exec(glBindTexture,GL_TEXTURE_2D, m_id);
        gl_exec(glTexImage2D, GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, nullptr);
        gl_exec(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterType);
        gl_exec(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterType);
        gl_exec(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        gl_exec(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        gl_exec(glFramebufferTexture2D, GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, m_id, 0);
    }

    render_target::~render_target()
    {
        //gl_exec(glDeleteTextures, 1, &m_id);
    }

    render_target::render_target(render_target&& move) noexcept
        : m_id(move.m_id)
    {
        move.m_id = 0;
    }

    render_target& render_target::operator=(render_target&& move) noexcept
    {
        if (this == &move)
            return *this;

        gl_exec(glDeleteTextures, 1, &m_id);
        m_id = move.m_id;
        move.m_id = 0;

        return *this;
    }
}

