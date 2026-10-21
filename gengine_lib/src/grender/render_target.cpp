#include "stdafx.h"

#include "grender/render_target.h"

#include "grender/utils.h"

#include "gcore/resource_library.h"

namespace grender
{
    render_target::render_target(gcore::resource_library& library, GLint internalFormat, GLenum format, GLenum type, GLenum attachment, GLenum filterType, GLsizei width, GLsizei height)
    {
        GLuint id = 0;
        gl_exec(glGenTextures,1, &id);
        gl_exec(glBindTexture,GL_TEXTURE_2D, id);
        gl_exec(glTexImage2D, GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, nullptr);
        gl_exec(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterType);
        gl_exec(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterType);
        gl_exec(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        gl_exec(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        gl_exec(glFramebufferTexture2D, GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, id, 0);

        std::unique_ptr<runtime_texture> run_tex = std::make_unique<runtime_texture>();
        run_tex->set_id(id);
        m_texture = library.add_runtime_resource(std::move(run_tex));
    }
}

