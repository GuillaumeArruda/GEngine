#pragma once

#include <GL/glew.h>

namespace grender
{
    struct render_target
    {
        render_target() {}
        render_target(GLint internalFormat, GLenum format, GLenum type, GLenum attachment, GLenum filterType, GLsizei width, GLsizei height);
        ~render_target();

        render_target(render_target&& move) noexcept;
        render_target& operator=(render_target&& move) noexcept;

        render_target(render_target const&) = delete;
        render_target& operator=(render_target const&) = delete;

        GLuint get_id() const { return m_id; }

    private:
        GLuint m_id = 0;
    };
}