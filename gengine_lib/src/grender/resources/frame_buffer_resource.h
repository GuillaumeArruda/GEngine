#pragma once

#include <gl/gl.h>

#include "gcore/resource.h"

namespace grender
{
    struct frame_buffer_resource : gcore::resource
    {
        bool do_load_async() override;
        bool do_load_sync() override;
        void do_unload() override;

        GLuint get_fbo() const { return m_fbo; }
        void set_fbo(GLuint fbo) { assert(m_fbo == 0); m_fbo = fbo; }
    protected:
        GLuint m_fbo = 0;
    };
}