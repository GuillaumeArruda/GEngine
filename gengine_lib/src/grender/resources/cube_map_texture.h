#pragma once

#include <array>
#include <GL/glew.h>

#include "gcore/resource.h"

namespace grender
{
    struct cube_map_texture : gcore::resource
    {
        void process(gserializer::serializer& serializer) override;
        
        void load() override;
        void unload() override;

        GLuint get_id() const { return m_texture_id; };
    private:
        std::array<std::filesystem::path,6> m_texture_files;
        GLuint m_texture_id;
        GSERIALIZER_DECLARE_SUBCLASS_FACTORY_REGISTRATION();
    };
}