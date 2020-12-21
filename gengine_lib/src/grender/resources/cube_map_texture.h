#pragma once

#include <array>
#include <GL/glew.h>

#include "gcore/resource.h"

namespace grender
{
    struct cube_map_texture : gcore::resource
    {
        void process(gserializer::serializer& serializer) override;
        bool need_async_load() const override { return true; }

        GLuint get_id() const { return m_texture_id; };

    private:
        bool do_load_async() override;
        bool do_load_sync() override { return true; };
        void do_unload() override;

        std::array<std::filesystem::path,6> m_texture_files;
        GLuint m_texture_id;
        GSERIALIZER_DECLARE_SUBCLASS_FACTORY_REGISTRATION();
    };
}