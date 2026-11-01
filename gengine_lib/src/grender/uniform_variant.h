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
            sampler_1d_array,
            sampler_1d_shadow,
            sampler_1d_array_shadow,
            sampler_2d,
            sampler_2d_shadow,
            sampler_2d_array,
            sampler_2d_array_shadow,
            sampler_3d,
            sampler_cube,
            sampler_cube_shadow,
            invalid,
        };

        struct vtable
        {
            void (*m_construct)(uniform_variant&) = nullptr;
            void (*m_destroy)(uniform_variant&) = nullptr;
            void (*m_copy_array)(uniform_variant&, void const*, std::size_t) = nullptr;
            void (*m_move)(uniform_variant&&, uniform_variant&) = nullptr;
            void (*m_process)(gserializer::serializer&, uniform_variant&) = nullptr;
            void (*m_apply)(uniform_variant const&, GLint, GLint) = nullptr;
        };

        struct pointer_info
        {
            void* m_ptr = nullptr;
            std::size_t m_size = 0;
        };

        template<class Type>
        static type get_type_enum_from_type()
        {
            if constexpr (std::is_same_v<Type, bool>) return type::glbool;
            else if constexpr (std::is_same_v<Type, GLint>) return type::glint;
            else if constexpr (std::is_same_v<Type, GLuint>) return type::gluint;
            else if constexpr (std::is_same_v<Type, float>) return type::glfloat; 
            else if constexpr (std::is_same_v<Type, double>) return type::gldouble;
            else if constexpr (std::is_same_v<Type, glm::bvec2>) return type::bvec2;
            else if constexpr (std::is_same_v<Type, glm::bvec3>) return type::bvec3;
            else if constexpr (std::is_same_v<Type, glm::bvec4>) return type::bvec4;
            else if constexpr (std::is_same_v<Type, glm::ivec2>) return type::ivec2;
            else if constexpr (std::is_same_v<Type, glm::ivec3>) return type::ivec3;
            else if constexpr (std::is_same_v<Type, glm::ivec4>) return type::ivec4;
            else if constexpr (std::is_same_v<Type, glm::uvec2>) return type::uvec2;
            else if constexpr (std::is_same_v<Type, glm::uvec3>) return type::uvec3;
            else if constexpr (std::is_same_v<Type, glm::uvec4>) return type::uvec4;
            else if constexpr (std::is_same_v<Type, glm::vec2>) return type::vec2;
            else if constexpr (std::is_same_v<Type, glm::vec3>) return type::vec3;
            else if constexpr (std::is_same_v<Type, glm::vec4>) return type::vec4;
            else if constexpr (std::is_same_v<Type, glm::dvec2>) return type::dvec2;
            else if constexpr (std::is_same_v<Type, glm::dvec3>) return type::dvec3;
            else if constexpr (std::is_same_v<Type, glm::dvec4>) return type::dvec4;
            else if constexpr (std::is_same_v<Type, glm::mat2>) return type::mat22;
            else if constexpr (std::is_same_v<Type, glm::mat2x3>) return type::mat23;
            else if constexpr (std::is_same_v<Type, glm::mat2x4>) return type::mat24;
            else if constexpr (std::is_same_v<Type, glm::mat3x2>) return type::mat32;
            else if constexpr (std::is_same_v<Type, glm::mat3>) return type::mat33;
            else if constexpr (std::is_same_v<Type, glm::mat3x4>) return type::mat34;
            else if constexpr (std::is_same_v<Type, glm::mat4x2>) return type::mat42;
            else if constexpr (std::is_same_v<Type, glm::mat4x3>) return type::mat43;
            else if constexpr (std::is_same_v<Type, glm::mat4>) return type::mat44;
            else if constexpr (std::is_same_v<Type, gcore::resource_handle<texture>>) return type::sampler_2d;
            else return type::invalid;
        }

        uniform_variant() : m_max_size(1), m_type(type::invalid) {}
        uniform_variant(type type, GLint max_size);
        uniform_variant(gtl::span<gcore::resource_handle<texture> const> values, type type);
        template<class Type>
        uniform_variant(gtl::span<Type const> values)
            : m_max_size(static_cast<uint32_t>(std::max(values.size(), 1uz)))
            , m_type(get_type_enum_from_type<Type>())
        {
            static_assert(!std::is_same_v<Type, gcore::resource_handle<texture>>, "Need to use the constructor that specify the texture type");
            vtable const table = get_vtable(m_type);
            table.m_construct(*this);
            table.m_copy_array(*this, values.data(), values.size());
        }

        uniform_variant(uniform_variant const& copy) noexcept;
        uniform_variant& operator=(uniform_variant const& copy) noexcept;

        uniform_variant(uniform_variant&& move) noexcept;
        uniform_variant& operator=(uniform_variant&&) noexcept;

        ~uniform_variant();

        template<class Type>
        void set_values(gtl::span<Type const> values)
        {
            if (m_type == get_type_enum_from_type<Type>())
            {
                get_vtable(m_type).m_copy_array(*this, values.data(), values.size());
            }
        }

        void apply(GLint location, GLint texture_unit) const;
        void process(gserializer::serializer& serializer);
        type get_type() const { return m_type; }

        std::uint32_t get_max_size() const { return m_max_size; }
        static GLint get_gl_texture_type(type type);
        
        pointer_info& get_ptr_info() { assert(m_max_size > 1); return *static_cast<pointer_info*>(get_buffer()); }
        pointer_info const& get_ptr_info() const { assert(m_max_size > 1); return *static_cast<pointer_info const*>(get_buffer()); }
        void* get_buffer() { return m_inline_buffer; }
        void const* get_buffer() const { return m_inline_buffer; }
        bool is_texture() const;

    private:
        static vtable get_vtable(type type);

        alignas(alignof(glm::mat4)) char m_inline_buffer[sizeof(glm::mat4)];
        std::uint32_t m_max_size = 1;
        type m_type;
    };

    uniform_variant::type get_type_from_gl_enum(GLenum enum_type);
}