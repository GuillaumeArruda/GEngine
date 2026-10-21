#pragma once

#include <GL/glew.h>

#include "gcore/resource_handle.h"
#include "grender/resources/runtime_texture.h"

namespace grender
{
    struct render_target
    {
        render_target() {}
        render_target(gcore::resource_library& library, GLint internalFormat, GLenum format, GLenum type, GLenum attachment, GLenum filterType, GLsizei width, GLsizei height);

        render_target(render_target&& move) noexcept = default;
        render_target& operator=(render_target&& move) noexcept = default;

        render_target(render_target const&) = delete;
        render_target& operator=(render_target const&) = delete;

        gcore::resource_handle<runtime_texture> get_texture() const { return m_texture; }

    private:
        gcore::resource_handle<runtime_texture> m_texture;
    };
}