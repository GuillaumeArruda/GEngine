#include "stdafx.h"


#undef _UNICODE
#include <IL\il.h>
#define _UNICODE

#include "grender/resources/cube_map_texture.h"

#include "gserializer/serializer.h"

#include "grender/utils.h"


GSERIALIZER_DEFINE_SUBCLASS_FACTORY_REGISTRATION(grender::cube_map_texture);

namespace grender
{
    void cube_map_texture::process(gserializer::serializer& serializer)
    {
        resource::process(serializer);
        serializer.process("positive_x_file_path", m_texture_files[0]);
        serializer.process("positive_y_file_path", m_texture_files[1]);
        serializer.process("positive_z_file_path", m_texture_files[2]);
        serializer.process("negative_x_file_path", m_texture_files[3]);
        serializer.process("negative_y_file_path", m_texture_files[4]);
        serializer.process("negative_z_file_path", m_texture_files[5]);
    }

    void cube_map_texture::load()
    {
        gl_exec(glGenTextures, 1, &m_texture_id);
        gl_exec(glBindTexture, GL_TEXTURE_CUBE_MAP, m_texture_id);
        GLenum const target[] =
        { GL_TEXTURE_CUBE_MAP_POSITIVE_X,
            GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
            GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
            GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
            GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
            GL_TEXTURE_CUBE_MAP_NEGATIVE_Z };

        for (int i = 0; i < 6; i++)
        {
            ILuint imageId = ilGenImage();

            ilBindImage(imageId);
            ILboolean success = ilLoadImage(m_texture_files[i].string().data());
            if (!success)
            {
                std::cerr << "Error while loading the texture file: " << m_texture_files[i] << std::endl;
                return;
            }

            int const width = ilGetInteger(IL_IMAGE_WIDTH);
            int const height = ilGetInteger(IL_IMAGE_HEIGHT);
            int const bytePerPixel = ilGetInteger(IL_IMAGE_BPP);
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
            gl_exec(glTexImage2D, target[i], 0, format, width, height, 0, ilGetInteger(IL_IMAGE_FORMAT), GL_UNSIGNED_BYTE, ilGetData());
            ilDeleteImage(imageId);
        }
        gl_exec(glTexParameteri, GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        gl_exec(glTexParameteri, GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        gl_exec(glTexParameteri, GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        gl_exec(glTexParameteri, GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        gl_exec(glTexParameteri, GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        gl_exec(glFinish);
        return;
    }

    void cube_map_texture::unload()
    {
        gl_exec(glDeleteTextures, 1, &m_texture_id);
        m_texture_id = 0;
    }


}

