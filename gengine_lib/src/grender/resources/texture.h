#pragma once

#include <filesystem>

#include "gcore/resource.h"

#include "GL/glew.h"

namespace grender
{
    struct texture : gcore::resource
    {
        ~texture();

        GLuint get_id() const { return m_texture_id; }
    protected:
        GLuint m_texture_id = 0;
    };

}