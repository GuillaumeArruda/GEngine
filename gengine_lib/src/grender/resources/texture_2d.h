#pragma once

#include "grender/resources/texture.h"

namespace grender
{
    struct texture_2d : texture
    {
        texture_2d() = default;
        ~texture_2d();

        texture_2d(texture_2d&& move) noexcept;
        texture_2d& operator=(texture_2d&& move) noexcept;

        texture_2d(texture_2d const&) = delete;
        texture_2d& operator=(texture_2d const&) = delete;

        void process(gserializer::serializer & serializer) override;

        GLuint get_id() const { return m_texture_id; }
        void set_filepath(std::filesystem::path path) { m_file_path = std::move(path); }
    private:
        bool do_load_async() override;
        bool do_load_sync() override { return true; };
        void do_unload() override;

        std::filesystem::path m_file_path;
        GSERIALIZER_DECLARE_SUBCLASS_FACTORY_REGISTRATION();
    };
}