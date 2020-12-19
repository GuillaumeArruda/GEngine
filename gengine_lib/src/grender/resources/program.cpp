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

    void program_uniform_state::apply(gcore::resource_library& lib) const 
    {
        for (GLint i = 0; i < static_cast<GLint>(m_uniforms.size()); ++i)
        {
            m_uniforms[i].m_uniform.apply(i, lib);
        }
    }

    void program_uniform_state::bind(GLint program_id)
    {
        m_uniforms.clear();
        GLint number_of_uniform = 0;
        gl_exec(glGetProgramiv, program_id, GL_ACTIVE_UNIFORMS, &number_of_uniform);
        m_uniforms.reserve(number_of_uniform);
        for (GLint i = 0; i < number_of_uniform; ++i)
        {
            constexpr GLsizei buffer_size = 256;
            GLchar buffer[buffer_size];
            GLsizei length;
            GLenum type;
            GLint size;
            gl_exec(glGetActiveUniform, program_id, i, buffer_size, &length, &size, &type, buffer);
            uniform& u = m_uniforms.emplace_back();
            u.m_name.reserve(length);
            u.m_name = buffer;
            u.m_uniform = uniform_variant(get_type_from_gl_enum(type));
        }
    }

    void program_uniform_state::set_uniform(GLint location, uniform_variant const& variant)
    {
        if (location >= m_uniforms.size() )
        {
            return;
        }

        if (m_uniforms[location].m_uniform.get_type() == variant.get_type())
        {
            m_uniforms[location].m_uniform = variant;
        }
    }

    void program_uniform_state::process(gserializer::serializer& serializer)
    {
        serializer.process("uniforms", m_uniforms, "Uniform");
    }

    void program_uniform_state::reconcile(program_uniform_state const& state)
    {
        std::vector<uniform> new_state = state.m_uniforms;
        for (auto const& old_uniform : m_uniforms)
        {
            auto it = std::find_if(new_state.begin(), new_state.end(),
                [&](uniform const& u) { return old_uniform.m_name == u.m_name && old_uniform.m_uniform.get_type() == u.m_uniform.get_type(); });
            if (it != new_state.end())
            {
                (*it).m_uniform = old_uniform.m_uniform;
            }
        }
        m_uniforms = std::move(new_state);
    }

    GLint program_uniform_state::get_uniform_location(std::string_view name) const
    {
        auto const it = std::find_if(m_uniforms.begin(), m_uniforms.end(), [&](uniform const& u) { return u.m_name == name; });
        if (it != m_uniforms.end())
        {
            return static_cast<GLint>(std::distance(m_uniforms.begin(), it));
        }
        return -1;
    }

    void program::process(gserializer::serializer& serializer)
    {
        resource::process(serializer);
        serializer.process("shaders", m_shaders_file, "Shader file");
        serializer.process("default uniforms", m_default_state);
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
        program_uniform_state new_state;
        new_state.bind(m_program_id);
        m_default_state.reconcile(new_state);
    }

    void program::unload()
    {
        glDeleteProgram(m_program_id);
        m_shaders.clear();
        m_program_id = 0;
    }

    void program::activate() const
    {
        gl_exec(glUseProgram, m_program_id);
    }

    void program_uniform_state::uniform::process(gserializer::serializer& serializer)
    {
        serializer.process("name", m_name);
        m_uniform.process(serializer);
    }

}
