#include "stdafx.h"

#include <fmt/format.h>

#include "grender/resources/texture_2d.h"

#include "gserializer/serializer.h"
#include "gserializer/serializers/binary_file_serializer.h"

#include "grender/utils.h"
#include "grender/dds_image_data.h"

GSERIALIZER_DEFINE_SUBCLASS_FACTORY_REGISTRATION(grender::texture_2d)

namespace grender
{
    texture_2d::~texture_2d() = default;

    texture_2d::texture_2d(texture_2d&& move) noexcept = default;

    texture_2d& texture_2d::operator=(texture_2d&& move) noexcept = default;

    void texture_2d::process(gserializer::serializer & serializer)
    {
        texture::process(serializer);
        serializer.process("file", m_file_path);
    }

    bool texture_2d::do_load_async()
    {
        dds_image_data data;
        gserializer::binary_file_reader reader(m_file_path.string().data());
        data.process(reader);

        if (!data.is_valid())
        {
            std::cerr << fmt::format("Error while loading file: {}. It is not a valid .dds file.\n", m_file_path.string());
            return false;
        }

        gl_exec(glGenTextures, 1, &m_texture_id);
        gl_exec(glBindTexture, GL_TEXTURE_2D, m_texture_id);
        gl_exec(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        gl_exec(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        gl_exec(glCompressedTexImage2D, GL_TEXTURE_2D, 0, data.get_opengl_format(), data.m_header.m_width, data.m_header.m_height, 0, static_cast<GLsizei>(data.m_data.size()), data.m_data.data());
        gl_exec(glFinish);
        return true;
    }

    void texture_2d::do_unload()
    {
        gl_exec(glDeleteTextures, 1, &m_texture_id);
    }
}

