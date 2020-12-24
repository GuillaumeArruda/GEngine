#include "stdafx.h"

#include <fmt/format.h>

#include "grender/resources/cube_map_texture.h"

#include "gserializer/serializer.h"
#include "gserializer/serializers/binary_file_serializer.h"

#include "grender/utils.h"
#include "grender/dds_image_data.h"

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

    bool cube_map_texture::do_load_async()
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
            dds_image_data data;
            gserializer::binary_file_reader reader(m_texture_files[i].string().data());
            data.process(reader);

            if (!data.is_valid())
            {
                gl_exec(glDeleteTextures, 1, &m_texture_id);
                std::cerr << fmt::format("Error while loading file: {}. It is not a valid .dds file.\n", m_texture_files[i].string());
                return false;
            }

            gl_exec(glCompressedTexImage2D, target[i], 0, data.get_opengl_format(), data.m_header.m_width, data.m_header.m_height, 0, static_cast<GLsizei>(data.m_data.size()), data.m_data.data());
        }
        gl_exec(glTexParameteri, GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        gl_exec(glTexParameteri, GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        gl_exec(glTexParameteri, GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        gl_exec(glTexParameteri, GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        gl_exec(glTexParameteri, GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        gl_exec(glFinish);
        return true;
    }

    void cube_map_texture::do_unload()
    {
        gl_exec(glDeleteTextures, 1, &m_texture_id);
        m_texture_id = 0;
    }


}

