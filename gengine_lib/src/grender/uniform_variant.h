#pragma once

#include <glm/glm.hpp>
#include <GL/glew.h>

#include "gtl/uuid.h" 

#include "gcore/resource_handle.h"
#include "grender/resources/texture.h"

namespace gcore
{
    struct resource_library;
}

namespace gserializer
{
    struct serializer;
}

namespace grender
{
    struct uniform_variant
    {
        enum class type
        {
            glbool,
            glint,
            gluint,
            glfloat,
            gldouble,
            bvec2,
            bvec3,
            bvec4,
            ivec2,
            ivec3,
            ivec4,
            uvec2,
            uvec3,
            uvec4,
            vec2,
            vec3,
            vec4,
            dvec2,
            dvec3,
            dvec4,
            mat22,
            mat23,
            mat24,
            mat32,
            mat33,
            mat34,
            mat42,
            mat43,
            mat44,
            sampler_1d,
            sampler_2d,
            sampler_3d,
            sampler_cube,
            invalid,
        };

        uniform_variant() : m_type(type::invalid) {}
        uniform_variant(type type);
        uniform_variant(bool value) : m_bool(value), m_type(type::glbool) {}
        uniform_variant(GLint value) : m_int(value), m_type(type::glint) {}
        uniform_variant(GLuint value) : m_uint(value), m_type(type::gluint) {}
        uniform_variant(float value) : m_float(value), m_type(type::glfloat) {}
        uniform_variant(double value) : m_double(value), m_type(type::gldouble) {}
        uniform_variant(glm::bvec2 value) : m_bvec2(value), m_type(type::bvec2) {}
        uniform_variant(glm::bvec3 value) : m_bvec3(value), m_type(type::bvec3) {}
        uniform_variant(glm::bvec4 value) : m_bvec4(value), m_type(type::bvec4) {}
        uniform_variant(glm::ivec2 value) : m_ivec2(value), m_type(type::ivec2) {}
        uniform_variant(glm::ivec3 value) : m_ivec3(value), m_type(type::ivec3) {}
        uniform_variant(glm::ivec4 value) : m_ivec4(value), m_type(type::ivec4) {}
        uniform_variant(glm::uvec2 value) : m_uvec2(value), m_type(type::uvec2) {}
        uniform_variant(glm::uvec3 value) : m_uvec3(value), m_type(type::uvec3) {}
        uniform_variant(glm::uvec4 value) : m_uvec4(value), m_type(type::uvec4) {}
        uniform_variant(glm::vec2 value) : m_vec2(value), m_type(type::vec2) {}
        uniform_variant(glm::vec3 value) : m_vec3(value), m_type(type::vec3) {}
        uniform_variant(glm::vec4 value) : m_vec4(value), m_type(type::vec4) {}
        uniform_variant(glm::dvec2 value) : m_dvec2(value), m_type(type::dvec2) {}
        uniform_variant(glm::dvec3 value) : m_dvec3(value), m_type(type::dvec3) {}
        uniform_variant(glm::dvec4 value) : m_dvec4(value), m_type(type::dvec4) {}
        uniform_variant(glm::mat2x2 const& value) : m_mat22(value), m_type(type::mat22) {}
        uniform_variant(glm::mat2x3 const& value) : m_mat23(value), m_type(type::mat23) {}
        uniform_variant(glm::mat2x4 const& value) : m_mat24(value), m_type(type::mat24) {}
        uniform_variant(glm::mat3x2 const& value) : m_mat32(value), m_type(type::mat32) {}
        uniform_variant(glm::mat3x3 const& value) : m_mat33(value), m_type(type::mat33) {}
        uniform_variant(glm::mat3x4 const& value) : m_mat34(value), m_type(type::mat34) {}
        uniform_variant(glm::mat4x2 const& value) : m_mat42(value), m_type(type::mat42) {}
        uniform_variant(glm::mat4x3 const& value) : m_mat43(value), m_type(type::mat43) {}
        uniform_variant(glm::mat4x4 const& value) : m_mat44(value), m_type(type::mat44) {}
        uniform_variant(gcore::resource_handle<texture> texture, type texture_type) : m_texture_info{ 0, std::move(texture), 0u }, m_type(texture_type) {}
        uniform_variant(gtl::uuid texture_id, type texture_type) : m_texture_info{ texture_id, std::shared_ptr<gcore::resource_proxy>(), 0u }, m_type(texture_type) {}
        uniform_variant(GLuint texture_id, type texture_type) : m_texture_info{gtl::uuid(),std::shared_ptr<gcore::resource_proxy>(), texture_id}, m_type(texture_type) {}

        uniform_variant(uniform_variant const& copy) noexcept;
        uniform_variant& operator=(uniform_variant const& copy) noexcept;

        uniform_variant(uniform_variant&& move) noexcept;
        uniform_variant& operator=(uniform_variant&&) noexcept;

        ~uniform_variant();


        void apply(GLint location) const;
        void process(gserializer::serializer& serializer);
        type get_type() const { return m_type; }

    private:
        void default_construct();
        void destroy();
        void move(uniform_variant&& move);
        void copy(uniform_variant const& copy);

        GLint get_gl_texture_type() const;

        struct texture_info
        {
            gtl::uuid m_texture_uuid;
            gcore::resource_handle<texture> m_texture;
            GLuint m_texture_id = 0;
        };

        union
        {
            bool m_bool;
            GLint m_int;
            GLuint m_uint;
            float m_float;
            double m_double;
            glm::bvec2 m_bvec2;
            glm::bvec3 m_bvec3;
            glm::bvec4 m_bvec4;
            glm::ivec2 m_ivec2;
            glm::ivec3 m_ivec3;
            glm::ivec4 m_ivec4;
            glm::uvec2 m_uvec2;
            glm::uvec3 m_uvec3;
            glm::uvec4 m_uvec4;
            glm::vec2 m_vec2;
            glm::vec3 m_vec3;
            glm::vec4 m_vec4;
            glm::dvec2 m_dvec2;
            glm::dvec3 m_dvec3;
            glm::dvec4 m_dvec4;
            glm::mat2x2 m_mat22;
            glm::mat2x3 m_mat23;
            glm::mat2x4 m_mat24;
            glm::mat3x2 m_mat32;
            glm::mat3x3 m_mat33;
            glm::mat3x4 m_mat34;
            glm::mat4x2 m_mat42;
            glm::mat4x3 m_mat43;
            glm::mat4x4 m_mat44;
            texture_info m_texture_info;
        };
        type m_type;
    };

    uniform_variant::type get_type_from_gl_enum(GLenum enum_type);
}