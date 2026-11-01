#include <stdafx.h>

#include "grender/resources/runtime_texture.h"
#include "grender/utils.h"

namespace grender
{
    runtime_texture::runtime_texture()
    {
        set_name("runtime texture");
    }

    bool grender::runtime_texture::do_load_sync()
    {
        return true;
    }

    void runtime_texture::do_unload()
    {
        gl_exec(glDeleteTextures, 1, &m_texture_id);
        m_texture_id = 0;
    }
}