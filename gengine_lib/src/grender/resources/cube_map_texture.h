#pragma once

#include <array>
#include <GL/glew.h>

#include "grender/resources/texture.h"

namespace grender
{
    struct cube_map_texture : texture
    {
        void process(gserializer::serializer& serializer) override;

    private:
        bool do_load_async() override;
        bool do_load_sync() override { return true; };
        void do_unload() override;

        std::array<std::filesystem::path,6> m_texture_files;
        GSERIALIZER_DECLARE_SUBCLASS_FACTORY_REGISTRATION();
    };
}