#include "stdafx.h"
#include "grender/resources/texture.h"

namespace grender
{
    texture::~texture()
    {
    }

    texture::texture(texture&& move) noexcept
        : resource(std::move(move))
        , m_texture_id(move.m_texture_id)
    {
        move.m_texture_id = 0;
    }

    texture& texture::operator=(texture&& move) noexcept
    {
        if (this == &move)
            return *this;

        resource::operator=(std::move(move));
        unload();
        m_texture_id = move.m_texture_id;
        move.m_texture_id = 0;
        return *this;
    }
}

