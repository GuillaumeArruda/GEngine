#pragma once

#include "grender/resources/texture.h"

namespace grender
{
    struct runtime_texture : texture
    {
        void set_id(GLuint id) { m_texture_id = id; }
        bool do_load_sync() override;
        void do_unload() override;
    };
}