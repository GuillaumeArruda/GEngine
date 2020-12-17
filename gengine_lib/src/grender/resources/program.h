#pragma once

#include <string_view>
#include <GL/glew.h>

#include "gcore/resource.h"

#include "gserializer/enum.h"

namespace grender
{
    struct shader
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

        shader(type type, std::string_view source_code);
        ~shader();

        shader(shader&& move) noexcept;
        shader& operator=(shader&& move) noexcept;

        shader(shader const&) = delete;
        shader& operator=(shader const&) = delete;

        GLuint get_id() const noexcept { return m_shader_id; }
    private:
        type m_type;
        GLuint m_shader_id = 0;
    };

    GSERIALIZER_FORWARD_DECLARE_ENUM_SERIALIZATION(shader::type);

    struct program : gcore::resource
    {
        void process(gserializer::serializer& serializer) override;
        void load() override;
        
        void activate();
        GLuint get_id() const noexcept { return m_program_id; }
    private:
        std::unordered_map<shader::type, std::string> m_shaders_file;
        std::vector<shader> m_shaders;
        GLuint m_program_id = 0;
        GSERIALIZER_DECLARE_SUBCLASS_FACTORY_REGISTRATION();
    };

}
