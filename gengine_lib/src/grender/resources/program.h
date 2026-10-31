#pragma once

#include <string_view>
#include <filesystem>
#include <GL/glew.h>

#include "gcore/resource.h"

#include "gserializer/enum.h"

#include "grender/uniform_variant.h"
#include "grender/resources/shader.h"

namespace gcore
{
    struct resource_library;
}

namespace grender
{
    struct program_uniform_state
    {
        void apply() const;
        void bind(GLint program_id);

        template<class Type>
        void set_uniform(GLint location, gtl::span<Type const> values)
        {
            if (location >= m_uniforms.size())
            {
                return;
            }

            m_uniforms[location].m_uniform.set_values(values);
        }

        template<class Type>
        void set_uniform(std::string_view name, gtl::span<Type const> values)
        {
            set_uniform(get_uniform_location(name), values);
        }
        void set_uniform(GLint location, uniform_variant&& variant);

        uniform_variant::type get_type_at_index(std::size_t index) const;
        std::string const& get_name_at_index(std::size_t index) const;
        void process(gserializer::serializer& serializer);
        void reconcile(program_uniform_state const& state);
        void copy_matching_uniform(program_uniform_state const& state);
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
        enum associated_program
        {
            directional_shadow,
            spot_light_shadow,
            point_light_shadow,
            count,
        };

        void process(gserializer::serializer& serializer) override;

        void activate() const;
        GLuint get_id() const noexcept { return m_program_id; }       
        program_uniform_state const& get_default_state() const { return m_default_state; }
        gcore::resource_handle<program> get_associated_program(associated_program type) const; 

    private:
        void do_unload() override;
        bool do_load_async() override;
        bool do_load_sync() override { return true; }

        program_uniform_state m_default_state;
        std::vector<gcore::resource_handle<shader>> m_shaders;
        std::array<gcore::resource_handle<program>, associated_program::count> m_associated_programs;
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
