#pragma once

#include <GL/glew.h>

#include "gcore/resource.h"


namespace grender
{
    struct shader : gcore::resource 
    {
        enum class type
        {
            vertex,
            fragment,
            geometry,
            tesselation_control,
            tesselation_evaluation,
            compute,
        };

        void process(gserializer::serializer& serializer) override;
        void do_unload() override;
        bool do_load_async() override;
        bool do_load_sync() override { return true; }

        GLuint get_id() const noexcept { return m_shader_id; }
        GLuint get_opengl_type() const noexcept;
    protected:
        type m_type;
        std::filesystem::path m_shader_path;
        GLuint m_shader_id = 0;

        GSERIALIZER_DECLARE_SUBCLASS_FACTORY_REGISTRATION();
    };
}