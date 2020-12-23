#pragma once

#include <filesystem>

#include "gcore/resource.h"

#include "GL/glew.h"

namespace grender
{
    struct texture : gcore::resource
    {
        texture() = default;
        ~texture();

        texture(texture&& move) noexcept;
        texture& operator=(texture&& move) noexcept;

        texture(texture const&) = delete;
        texture& operator=(texture const&) = delete;

        void process(gserializer::serializer& serializer) override;

        GLuint get_id() const { return m_texture_id; }

        void set_filepath(std::filesystem::path new_path) { m_file_path = std::move(new_path); }

    private:
        bool do_load_async() override;
        bool do_load_sync() override { return true; };
        void do_unload() override;

        std::filesystem::path m_file_path;
        GLuint m_texture_id = 0;
        GSERIALIZER_DECLARE_SUBCLASS_FACTORY_REGISTRATION();
    };

}