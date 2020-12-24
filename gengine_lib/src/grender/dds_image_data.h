#pragma once

#include <vector>

namespace gserializer
{
    struct serializer;
    struct binary_file_reader;
}

namespace grender
{
    struct dds_image_data
    {
        struct pixel_format
        {
            std::uint32_t  m_size = 0;
            std::uint32_t  m_flags = 0;
            std::uint32_t  m_fourCC = 0;
            std::uint32_t  m_RGB_bit_count = 0;
            std::uint32_t  m_R_bit_mask = 0;
            std::uint32_t  m_G_bit_mask = 0;
            std::uint32_t  m_B_bit_mask = 0;
            std::uint32_t  m_A_bit_mask = 0;
            void process(gserializer::serializer& serializer);
        };

        struct header
        {
            std::uint32_t m_size = 0;
            std::uint32_t m_flags = 0;
            std::uint32_t m_height = 0;
            std::uint32_t m_width = 0;
            std::uint32_t m_pitch_or_linear_size = 0;
            std::uint32_t m_depth = 0;
            std::uint32_t m_mip_map_count = 0;
            std::uint32_t m_reserveds[11] = { 0 };
            pixel_format m_pixel_format;
            std::uint32_t m_caps = 0;
            std::uint32_t m_caps2 = 0;
            std::uint32_t m_caps3 = 0;
            std::uint32_t m_caps4 = 0;
            std::uint32_t m_reserved = 0;
            void process(gserializer::serializer& serializer);
        };

        bool is_valid() const;
        int get_opengl_format() const;

        std::uint32_t m_magic_header = 0;
        header m_header;
        std::vector<char> m_data;
        void process(gserializer::binary_file_reader& process);
    };
}