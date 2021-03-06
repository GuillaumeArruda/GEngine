#pragma once

#include <string_view>
#include <filesystem>
#include <GL/glew.h>

#include "gcore/resource.h"

#include "gserializer/enum.h"

#include "grender/uniform_variant.h"

namespace gcore
{
    struct resource_library;
}

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

    struct program_uniform_state
    {
        void apply() const;
        void bind(GLint program_id);
        void set_uniform(GLint location, uniform_variant const& variant);
        void set_uniform(std::string_view name, uniform_variant const& variant);
        uniform_variant::type get_type_at_index(std::size_t index) const;
        std::string const& get_name_at_index(std::size_t index) const;
        void process(gserializer::serializer& serializer);
        void reconcile(program_uniform_state const& state);
        GLint get_uniform_location(std::string_view name) const;
        std::size_t get_uniform_number() const { return m_uniforms.size(); }
    private:
        struct uniform
        {
            uniform_variant m_uniform;
            std::string m_name;
            void process(gserializer::serializer& serializer);
        };
        std::vector<uniform> m_uniforms;
    };

    struct program : gcore::resource
    {
        void process(gserializer::serializer& serializer) override;

        void activate() const;
        GLuint get_id() const noexcept { return m_program_id; }       
        program_uniform_state const& get_default_state() const { return m_default_state; }

    private:
        void do_unload() override;
        bool do_load_async() override;
        bool do_load_sync() override { return true; }

        program_uniform_state m_default_state;
        std::unordered_map<shader::type, std::filesystem::path> m_shaders_file;
        std::vector<shader> m_shaders;
        GLuint m_program_id = 0;
        GSERIALIZER_DECLARE_SUBCLASS_FACTORY_REGISTRATION();
    };

    struct program_pin_data
    {
        gcore::resource_handle<program> m_program;
        program_uniform_state m_state;
        void process(gserializer::serializer& serializer);
    };
}
