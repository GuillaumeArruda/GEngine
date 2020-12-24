#include "stdafx.h"
#include "grender/dds_image_data.h"

#include <GL/glew.h>

#include "gserializer/serializers/binary_file_serializer.h"


namespace
{
    std::uint32_t const dds_1_cc = 0x31545844U; // DXT1
    std::uint32_t const dds_3_cc = 0x33545844U; // DXT3
    std::uint32_t const dds_5_cc = 0x35545844U; // DXT5
}

namespace grender
{
    void dds_image_data::pixel_format::process(gserializer::serializer& serializer)
    {
        serializer.process("size", m_size);
        serializer.process("flags", m_flags);
        serializer.process("fourCC", m_fourCC);
        serializer.process("RGB_bit_count", m_RGB_bit_count);
        serializer.process("R_bit_mask", m_R_bit_mask);
        serializer.process("G_bit_mask", m_G_bit_mask);
        serializer.process("B_bit_mask", m_B_bit_mask);
        serializer.process("A_bit_mask", m_A_bit_mask);
    }

    void dds_image_data::header::process(gserializer::serializer& serializer)
    {
        serializer.process("size", m_size);
        serializer.process("flags", m_flags);
        serializer.process("height", m_height);
        serializer.process("width", m_width);
        serializer.process("pitch_or_linear", m_pitch_or_linear_size);
        serializer.process("depth", m_depth);
        serializer.process("min_map_count", m_mip_map_count);
        assert(m_mip_map_count == 1);
        for (std::size_t i = 0; i < std::size(m_reserveds); ++i)
            serializer.process("reserved", m_reserveds[i]);
        serializer.process("pixel_format", m_pixel_format);
        serializer.process("caps1", m_caps);
        serializer.process("caps2", m_caps2);
        serializer.process("caps3", m_caps3);
        serializer.process("caps4", m_caps4);
        serializer.process("reserved", m_reserved);
    }
    void dds_image_data::process(gserializer::binary_file_reader& serializer)
    {
        serializer.process("magic_header", m_magic_header);
        serializer.process("header", m_header);
        m_data.resize(m_header.m_pitch_or_linear_size);
        serializer.read_data(m_data.size(), m_data.data());
    }

    int dds_image_data::get_opengl_format() const
    {
        switch (m_header.m_pixel_format.m_fourCC)
        {
        case dds_1_cc: return GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
        case dds_3_cc: return GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
        case dds_5_cc: return GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
        }
        return 0;
    }
    bool dds_image_data::is_valid() const
    {
        return m_magic_header == 0x20534444U;
    }
}

