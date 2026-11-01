#include "stdafx.h"

#include "grender/resources/program.h"
#include "grender/utils.h"

#include "gserializer/serializer.h"

GSERIALIZER_DEFINE_SUBCLASS_FACTORY_REGISTRATION(grender::program);

namespace grender
{
    void program_uniform_state::apply() const 
    {
        GLint location = 0;
        GLint texture_unit = 0;
        for (auto const& uniform : m_uniforms)
        {
            uniform.m_uniform.apply(location, texture_unit);
            location += uniform.m_uniform.get_max_size();
            texture_unit += uniform.m_uniform.is_texture() ? uniform.m_uniform.get_max_size() : 0;
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
            GLint max_size;
            gl_exec(glGetActiveUniform, program_id, i, buffer_size, &length, &max_size, &type, buffer);
            uniform& u = m_uniforms.emplace_back();
            u.m_name.reserve(length);
            u.m_name = std::string_view(buffer, length);
            u.m_uniform = uniform_variant(get_type_from_gl_enum(type), max_size);
        }
    }

    void program_uniform_state::set_uniform(GLint location, uniform_variant&& variant)
    {
        if (location >= m_uniforms.size())
        {
            return;
        }

        if (variant.get_type() == m_uniforms[location].m_uniform.get_type())
        {
            m_uniforms[location].m_uniform = std::move(variant);
        }
    }

    uniform_variant::type program_uniform_state::get_type_at_index(std::size_t index) const
    {
        return m_uniforms.at(index).m_uniform.get_type();
    }

    std::string const& program_uniform_state::get_name_at_index(std::size_t index) const
    {
        return m_uniforms.at(index).m_name;
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

    void program_uniform_state::copy_matching_uniform(program_uniform_state const& state)
    {
        for (auto& uni : m_uniforms)
        {
            auto it = std::find_if(state.m_uniforms.begin(), state.m_uniforms.end(),
                [&](uniform const& u) 
            { 
                return uni.m_name == u.m_name && uni.m_uniform.get_type() == u.m_uniform.get_type() && uni.m_uniform.get_max_size() == u.m_uniform.get_max_size(); 
            });
            if (it != state.m_uniforms.end())
            {
                uni.m_uniform = it->m_uniform;
            }
        }
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
        serializer.process("shaders", m_shaders, "shader");
        serializer.process("directional shadow", m_associated_programs[associated_program::directional_shadow]);
        serializer.process("spot light shadow", m_associated_programs[associated_program::spot_light_shadow]);
        serializer.process("point light shadow", m_associated_programs[associated_program::point_light_shadow]);
        serializer.process("default uniforms", m_default_state);
    }

    bool program::do_load_async()
    {
        m_program_id = glCreateProgram();

        for (auto& shader : m_shaders)
        {
            if (shader.is_loaded())
            {
                gl_exec(glAttachShader, m_program_id, shader->get_id());
            }
            else if (shader.is_failed())
            {
                return false;
            }
        }
        gl_exec(glLinkProgram, m_program_id);

        GLint isLinked = 0;
        gl_exec(glGetProgramiv, m_program_id, GL_LINK_STATUS, &isLinked);
        if (isLinked == GL_FALSE)
        {
            GLint logSize = 0;
            gl_exec(glGetProgramiv, m_program_id, GL_INFO_LOG_LENGTH, &logSize);
            if (logSize > 0)
            {
                auto log = std::make_unique<GLchar[]>(logSize);
                gl_exec(glGetProgramInfoLog, m_program_id, logSize, &logSize, log.get());
                std::cerr << log << "\n";
            }
            gl_exec(glDeleteProgram, m_program_id);
            m_program_id = 0;
            return false;
        }
        program_uniform_state new_state;
        new_state.bind(m_program_id);
        m_default_state.reconcile(new_state);

        gl_exec(glFinish);
        return true;
    }

    void program::do_unload()
    {
        gl_exec(glDeleteProgram, m_program_id);
        m_shaders.clear();
        m_program_id = 0;
    }

    void program::activate() const
    {
        gl_exec(glUseProgram, m_program_id);
    }

    gcore::resource_handle<program> program::get_associated_program(associated_program type) const
    {
        return m_associated_programs[type];
    }

    void program_uniform_state::uniform::process(gserializer::serializer& serializer)
    {
        serializer.process("name", m_name);
        m_uniform.process(serializer);
    }

    void program_pin_data::process(gserializer::serializer& serializer)
    {
        serializer.process("program", m_program);
        serializer.process("state", m_state);
    }

}
