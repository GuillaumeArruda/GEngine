#include "stdafx.h"

#undef _UNICODE
#include <IL\il.h>
#define _UNICODE

#include <filesystem>

#include "grender/resources/texture.h"

#include "gserializer/serializer.h"

#include "grender/utils.h"

GSERIALIZER_DEFINE_SUBCLASS_FACTORY_REGISTRATION(grender::texture)

namespace grender
{
    texture::~texture()
    {
        unload();
    }

    texture::texture(texture&& move) noexcept
        : m_texture_id(move.m_texture_id)
        , m_file_path(std::move(move.m_file_path))
    {
        move.m_texture_id = 0;
    }

    texture& texture::operator=(texture&& move) noexcept
    {
        if (this == &move)
            return *this;

        unload();
        m_file_path = std::move(move.m_file_path);
        m_texture_id = move.m_texture_id;
        move.m_texture_id = 0;
        return *this;
    }

    void texture::process(gserializer::serializer& serializer)
    {
        resource::process(serializer);
        serializer.process("file", m_file_path);
    }

    bool texture::do_load_async()
    {
        gl_exec(glGenTextures, 1, &m_texture_id);
        ILuint const imageId = ilGenImage();

        ilBindImage(imageId);
        ILboolean const success = ilLoadImage(m_file_path.string().data());
        if (!success)
        {
            gl_exec(glDeleteTextures, 1, &m_texture_id);
            m_texture_id = 0;
            std::cerr << "Error while loading the texture file: " << m_file_path << std::endl;
            return false;
        }

        auto const width = ilGetInteger(IL_IMAGE_WIDTH);
        auto const height = ilGetInteger(IL_IMAGE_HEIGHT);
        auto const bytePerPixel = ilGetInteger(IL_IMAGE_BPP);
        GLuint format;
        switch (bytePerPixel)
        {
        case 1:
            format = GL_R;
            break;
        case 2:
            format = GL_RG;
            break;
        case 3:
            format = GL_RGB;
            break;
        case 4:
            format = GL_RGBA;
            break;
        default:
            format = GL_RGBA;
            break;
        }
        gl_exec(glBindTexture, GL_TEXTURE_2D, m_texture_id);
        gl_exec(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        gl_exec(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        gl_exec(glPixelStorei, GL_UNPACK_ALIGNMENT, 1);
        gl_exec(glPixelStorei, GL_UNPACK_ROW_LENGTH, 0);
        gl_exec(glPixelStorei, GL_UNPACK_SKIP_PIXELS, 0);
        gl_exec(glPixelStorei, GL_UNPACK_SKIP_ROWS, 0);
        gl_exec(glPixelStorei, GL_UNPACK_SWAP_BYTES, GL_FALSE);
        gl_exec(glTexImage2D, GL_TEXTURE_2D, 0, format, width, height, 0, ilGetInteger(IL_IMAGE_FORMAT), ilGetInteger(IL_IMAGE_TYPE), ilGetData());
        ilDeleteImage(imageId);
        gl_exec(glFinish);
        return true;
    }

    void texture::do_unload()
    {
        gl_exec(glDeleteTextures, 1, &m_texture_id);
    }
}

