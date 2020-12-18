#include "stdafx.h"

#include "grender/resources/program.h"
#include "grender/utils.h"

#include "gserializer/serializer.h"

#include <fstream>

GSERIALIZER_DEFINE_SUBCLASS_FACTORY_REGISTRATION(grender::program);

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

    GLuint to_open_gl_shader_type(shader::type type)
    {
        switch (type)
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

    shader::shader(type type, std::string_view source_code)
        : m_type(type)
        , m_shader_id(glCreateShader(to_open_gl_shader_type(m_type)))
    {
        GLint size = static_cast<GLint>(source_code.size());
        const char* source = source_code.data();
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
        }
    }

    shader::~shader()
    {
        gl_exec(glDeleteShader, m_shader_id);
    }

    shader::shader(shader&& move) noexcept
        : m_type(move.m_type)
        , m_shader_id(move.m_shader_id)
    {
        move.m_shader_id = 0;
    }
    shader& shader::operator=(shader&& move) noexcept
    {
        if (this == &move)
            return *this;

        gl_exec(glDeleteShader, m_shader_id);
        m_shader_id = move.m_shader_id;
        move.m_shader_id = 0;
        return *this;
    }

    void program::process(gserializer::serializer& serializer)
    {
        resource::process(serializer);
        serializer.process("shaders", m_shaders_file);
    }

    void program::load()
    {
        m_program_id = glCreateProgram();

        for (auto& shader_file : m_shaders_file)
        {
            std::ifstream file(shader_file.second);
            if (!file.is_open())
                continue;

            std::string content;

            file.seekg(0, std::ios::end);
            content.reserve(file.tellg());
            file.seekg(0, std::ios::beg);

            content.assign((std::istreambuf_iterator<char>(file)),
                std::istreambuf_iterator<char>());

            shader& s = m_shaders.emplace_back(shader_file.first, content);
            gl_exec(glAttachShader, m_program_id, s.get_id());
        }
        gl_exec(glLinkProgram, m_program_id);

        GLint isLinked = 0;
        glGetProgramiv(m_program_id, GL_LINK_STATUS, &isLinked);
        if (isLinked == GL_FALSE)
        {
            GLint logSize = 0;
            glGetProgramiv(m_program_id, GL_INFO_LOG_LENGTH, &logSize);

            auto log = std::make_unique<GLchar[]>(logSize);
            glGetProgramInfoLog(m_program_id, logSize, &logSize, log.get());
            std::cerr << log << "\n";
        }
    }

    void program::unload()
    {
        glDeleteProgram(m_program_id);
        m_shaders.clear();
        m_program_id = 0;
    }

    void program::activate()
    {
        gl_exec(glUseProgram, m_program_id);
    }
}
