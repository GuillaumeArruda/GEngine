#include <stdafx.h>

#include <fstream>

#include "grender/resources/shader.h"
#include "grender/utils.h"

#include "gserializer/enum.h"
#include "gserializer/serializer.h"

GSERIALIZER_DEFINE_SUBCLASS_FACTORY_REGISTRATION(grender::shader);

namespace grender
{
    GSERIALIZER_BEGIN_ENUM_HANDLER(shader::type)
        GSERIALIZER_ADD_ENUM_ELEMENT(shader::type::vertex)
        GSERIALIZER_ADD_ENUM_ELEMENT(shader::type::fragment)
        GSERIALIZER_ADD_ENUM_ELEMENT(shader::type::geometry)
        GSERIALIZER_ADD_ENUM_ELEMENT(shader::type::tesselation_control)
        GSERIALIZER_ADD_ENUM_ELEMENT(shader::type::tesselation_evaluation)
        GSERIALIZER_ADD_ENUM_ELEMENT(shader::type::compute)
    GSERIALIZER_END_ENUM_HANDLER(shader::type)

    void shader::process(gserializer::serializer& serializer)
    {
        gcore::resource::process(serializer);
        serializer.process("type", m_type);
        serializer.process("path", m_shader_path);
    }

    void shader::do_unload()
    {
        gl_exec(glDeleteShader, m_shader_id);
    }

    GLuint shader::get_opengl_type() const noexcept
    {
        switch (m_type)
        {
        case shader::type::vertex:
            return GL_VERTEX_SHADER;
        case shader::type::fragment:
            return GL_FRAGMENT_SHADER;
        case shader::type::geometry:
            return GL_GEOMETRY_SHADER;
        case shader::type::tesselation_control:
            return GL_TESS_CONTROL_SHADER;
        case shader::type::tesselation_evaluation:
            return GL_TESS_EVALUATION_SHADER;
        case shader::type::compute:
        default:
            return GL_COMPUTE_SHADER;
        }
    }

    bool shader::do_load_async()
    {
        m_shader_id = gl_exec(glCreateShader, get_opengl_type());

        std::ifstream file(m_shader_path);
        if (!file.is_open())
            return false;

        std::string source_code;
        source_code.assign((std::istreambuf_iterator<char>(file)),
            std::istreambuf_iterator<char>());

        GLint const size = static_cast<GLint>(source_code.size());
        char const* source = source_code.data();
        gl_exec(glShaderSource, m_shader_id, 1, &source, &size);
        gl_exec(glCompileShader, m_shader_id);

        GLint success = 0;
        glGetShaderiv(m_shader_id, GL_COMPILE_STATUS, &success);
        if (success == GL_FALSE)
        {
            std::cerr << "Unable to compile shader:" << source_code;
            GLint logSize = 0;
            GLint length = 0;
            glGetShaderiv(m_shader_id, GL_INFO_LOG_LENGTH, &logSize);
            auto log = std::make_unique<GLchar[]>(logSize);
            glGetShaderInfoLog(m_shader_id, logSize, &length, log.get());
            std::cout << log << std::endl;
            return false;
        }

        return true;
    }

}
