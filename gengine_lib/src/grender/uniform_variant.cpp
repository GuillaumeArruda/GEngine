#include "stdafx.h"
#include "grender/uniform_variant.h"

#include <glm/gtc/type_ptr.hpp>

#include "grender/utils.h"

#include "gserializer/enum.h"
#include "gserializer/serializer.h"
#include "gserializer/gmath_serialization.h"

#include "gcore/resource_library.h"

#include "grender/resources/texture.h"
#include "grender/resources/cube_map_texture.h"

namespace grender
{
    GSERIALIZER_BEGIN_ENUM_HANDLER(uniform_variant::type)
        GSERIALIZER_ADD_ENUM_ELEMENT(uniform_variant::type::glbool)
        GSERIALIZER_ADD_ENUM_ELEMENT(uniform_variant::type::glint)
        GSERIALIZER_ADD_ENUM_ELEMENT(uniform_variant::type::gluint)
        GSERIALIZER_ADD_ENUM_ELEMENT(uniform_variant::type::glfloat)
        GSERIALIZER_ADD_ENUM_ELEMENT(uniform_variant::type::gldouble)
        GSERIALIZER_ADD_ENUM_ELEMENT(uniform_variant::type::bvec2)
        GSERIALIZER_ADD_ENUM_ELEMENT(uniform_variant::type::bvec3)
        GSERIALIZER_ADD_ENUM_ELEMENT(uniform_variant::type::bvec4)
        GSERIALIZER_ADD_ENUM_ELEMENT(uniform_variant::type::ivec2)
        GSERIALIZER_ADD_ENUM_ELEMENT(uniform_variant::type::ivec3)
        GSERIALIZER_ADD_ENUM_ELEMENT(uniform_variant::type::ivec4)
        GSERIALIZER_ADD_ENUM_ELEMENT(uniform_variant::type::uvec2)
        GSERIALIZER_ADD_ENUM_ELEMENT(uniform_variant::type::uvec3)
        GSERIALIZER_ADD_ENUM_ELEMENT(uniform_variant::type::uvec4)
        GSERIALIZER_ADD_ENUM_ELEMENT(uniform_variant::type::vec2)
        GSERIALIZER_ADD_ENUM_ELEMENT(uniform_variant::type::vec3)
        GSERIALIZER_ADD_ENUM_ELEMENT(uniform_variant::type::vec4)
        GSERIALIZER_ADD_ENUM_ELEMENT(uniform_variant::type::dvec2)
        GSERIALIZER_ADD_ENUM_ELEMENT(uniform_variant::type::dvec3)
        GSERIALIZER_ADD_ENUM_ELEMENT(uniform_variant::type::dvec4)
        GSERIALIZER_ADD_ENUM_ELEMENT(uniform_variant::type::mat22)
        GSERIALIZER_ADD_ENUM_ELEMENT(uniform_variant::type::mat23)
        GSERIALIZER_ADD_ENUM_ELEMENT(uniform_variant::type::mat24)
        GSERIALIZER_ADD_ENUM_ELEMENT(uniform_variant::type::mat32)
        GSERIALIZER_ADD_ENUM_ELEMENT(uniform_variant::type::mat33)
        GSERIALIZER_ADD_ENUM_ELEMENT(uniform_variant::type::mat34)
        GSERIALIZER_ADD_ENUM_ELEMENT(uniform_variant::type::mat42)
        GSERIALIZER_ADD_ENUM_ELEMENT(uniform_variant::type::mat43)
        GSERIALIZER_ADD_ENUM_ELEMENT(uniform_variant::type::mat44)
        GSERIALIZER_ADD_ENUM_ELEMENT(uniform_variant::type::sampler_1d)
        GSERIALIZER_ADD_ENUM_ELEMENT(uniform_variant::type::sampler_1d_array)
        GSERIALIZER_ADD_ENUM_ELEMENT(uniform_variant::type::sampler_1d_shadow)
        GSERIALIZER_ADD_ENUM_ELEMENT(uniform_variant::type::sampler_1d_array_shadow)
        GSERIALIZER_ADD_ENUM_ELEMENT(uniform_variant::type::sampler_2d)
        GSERIALIZER_ADD_ENUM_ELEMENT(uniform_variant::type::sampler_2d_array)
        GSERIALIZER_ADD_ENUM_ELEMENT(uniform_variant::type::sampler_2d_shadow)
        GSERIALIZER_ADD_ENUM_ELEMENT(uniform_variant::type::sampler_2d_array)
        GSERIALIZER_ADD_ENUM_ELEMENT(uniform_variant::type::sampler_3d)
        GSERIALIZER_ADD_ENUM_ELEMENT(uniform_variant::type::sampler_cube)
        GSERIALIZER_ADD_ENUM_ELEMENT(uniform_variant::type::sampler_cube_shadow)
    GSERIALIZER_END_ENUM_HANDLER(uniform_variant::type)

    uniform_variant::uniform_variant(type type, GLint max_size)
        : m_max_size(std::max(static_cast<std::uint32_t>(max_size), 1u))
        , m_type(type)
    {
        get_vtable(m_type).m_construct(*this);
    }

    uniform_variant::uniform_variant(gtl::span<gcore::resource_handle<texture> const> values, type type)
        : m_type(type)
        , m_max_size(std::max(static_cast<std::uint32_t>(values.size()), 1u))
    {
        vtable const table = get_vtable(m_type);
        table.m_construct(*this);
        table.m_copy_array(*this, values.data(), values.size());
    }

    uniform_variant::uniform_variant(uniform_variant const& copy_value) noexcept
        : m_type(copy_value.m_type)
        , m_max_size(copy_value.m_max_size)
    {
        vtable const table = get_vtable(m_type);
        table.m_construct(*this);
        if (copy_value.get_max_size() == 1)
        {
            table.m_copy_array(*this, copy_value.get_buffer(), 1);
        }
        else
        {
            pointer_info const info = copy_value.get_ptr_info();
            table.m_copy_array(*this, info.m_ptr, info.m_size);
        }
    }

    uniform_variant& uniform_variant::operator=(uniform_variant const& copy_value) noexcept
    {
        if (this == &copy_value)
            return *this;

        get_vtable(m_type).m_destroy(*this);

        m_type = copy_value.m_type;
        m_max_size = copy_value.m_max_size;
        vtable const table = get_vtable(m_type);
        table.m_construct(*this);
        if (copy_value.get_max_size() == 1)
        {
            table.m_copy_array(*this, copy_value.get_buffer(), 1);
        }
        else
        {
            pointer_info const info = copy_value.get_ptr_info();
            table.m_copy_array(*this, info.m_ptr, info.m_size);
        }
        return *this;
    }

    uniform_variant::uniform_variant(uniform_variant&& move_value) noexcept
        : m_max_size(move_value.m_max_size),
        m_type(move_value.m_type)
    {
        get_vtable(m_type).m_move(std::move(move_value), *this);
        move_value.m_type = type::invalid;
    }

    uniform_variant& uniform_variant::operator=(uniform_variant&& move_value) noexcept
    {
        if (this == &move_value)
            return *this;

        get_vtable(m_type).m_destroy(*this);
        m_type = move_value.m_type;
        m_max_size = move_value.m_max_size;
        get_vtable(m_type).m_move(std::move(move_value), *this);
        move_value.m_type = type::invalid;
        return *this;
    }

    uniform_variant::~uniform_variant()
    {
        get_vtable(m_type).m_destroy(*this);
    }

    void uniform_variant::process(gserializer::serializer& serializer)
    {
        using gserializer::process;
        type const old_type = m_type;
        std::uint32_t const old_max_size = m_max_size;
        serializer.process("type", m_type);
        serializer.process("max_size", m_max_size);
        
        vtable const table = get_vtable(m_type);
        if (old_type != m_type || m_max_size != old_max_size)
        {
            vtable const old_vtable = get_vtable(old_type);
            old_vtable.m_destroy(*this);
            table.m_construct(*this);
        }

        table.m_process(serializer, *this);
    }

    template<class T>
    struct get_array_type
    {
        using Type = T;
    };

    template<>
    struct get_array_type<bool>
    {
        using Type = GLint;
    };

    template<>
    struct get_array_type<glm::bvec2>
    {
        using Type = glm::ivec2;
    };

    template<>
    struct get_array_type<glm::bvec3>
    {
        using Type = glm::ivec3;
    };

    template<>
    struct get_array_type<glm::bvec4>
    {
        using Type = glm::ivec4;
    };

    template<class Type>
    void construct_uniform_variant(uniform_variant& variant)
    {
        if (variant.get_max_size() == 1)
        {
            new(variant.get_buffer()) Type();
        }
        else
        {
            using array_type = get_array_type<Type>::Type;
            auto* ptr_info = new(variant.get_buffer()) uniform_variant::pointer_info{ operator new(sizeof(array_type) * variant.get_max_size()), variant.get_max_size() };
            for (std::size_t i = 0; i < variant.get_max_size(); ++i)
            {
                new(static_cast<array_type*>(ptr_info->m_ptr) + i) Type();
            }
            ptr_info->m_size = variant.get_max_size();
        }
    }

    template<class Type>
    void destroy_uniform_variant(uniform_variant& variant)
    {
        if (variant.get_max_size() == 1)
        {
            static_cast<Type*>(variant.get_buffer())->~Type();
        }
        else
        {
            using array_type = get_array_type<Type>::Type;
            uniform_variant::pointer_info const ptr_info = variant.get_ptr_info();
            if (ptr_info.m_ptr != nullptr)
            {
                for (std::size_t i = 0; i < variant.get_max_size(); ++i)
                {
                    (static_cast<array_type*>(ptr_info.m_ptr) + i)->~array_type();
                }
                operator delete(ptr_info.m_ptr);
            }
            using info = uniform_variant::pointer_info;
            static_cast<uniform_variant::pointer_info*>(variant.get_buffer())->~info();
        }
    }

    template<class Type>
    void copy_array_uniform_variant(uniform_variant& variant, void const* data, std::size_t size)
    {
        if (size >= 1)
        {
            if (variant.get_max_size() == 1)
            {
                *static_cast<Type*>(variant.get_buffer()) = *static_cast<Type const*>(data);
            }
            else
            {
                using array_type = get_array_type<Type>::Type;
                uniform_variant::pointer_info& ptr_info = variant.get_ptr_info();
                ptr_info.m_size = std::min(size, static_cast<std::size_t>(variant.get_max_size()));
                for (std::size_t i = 0; i < ptr_info.m_size; ++i)
                {
                    *(static_cast<array_type*>(ptr_info.m_ptr) + i) = *(static_cast<Type const*>(data) + i);
                }
            }
        }
    }

    template<class Type>
    void move_uniform_variant(uniform_variant&& from, uniform_variant& to)
    {
        if (to.get_max_size() == 1)
        {
            new(to.get_buffer()) Type(std::move(*static_cast<Type*>(from.get_buffer())));
        }
        else
        {
            uniform_variant::pointer_info& from_info = from.get_ptr_info();
            to.get_ptr_info() = from_info;
            from_info.m_ptr = nullptr;
            from_info.m_size = 0;
        }
    }

    template<class Type>
    void process_uniform_variant(gserializer::serializer& serializer, uniform_variant& variant)
    {
        if (variant.get_max_size() == 1)
        {
            process(serializer, *static_cast<Type*>(variant.get_buffer()));
        }
        else
        {
            using array_type = get_array_type<Type>::Type;
            uniform_variant::pointer_info const ptr_info = variant.get_ptr_info();
            std::size_t max_size = variant.get_max_size();
            serializer.open_array("values", max_size);
            for (std::size_t i = 0; i < variant.get_max_size(); ++i)
            {
                serializer.open_array_element("value");
                if constexpr (std::is_same_v<array_type, Type>)
                {
                    process(serializer, *(static_cast<array_type*>(ptr_info.m_ptr) + i));
                }
                else
                {
                    Type value = *(static_cast<array_type*>(ptr_info.m_ptr) + i);
                    process(serializer, value);
                    *(static_cast<array_type*>(ptr_info.m_ptr) + i) = value;
                }
                serializer.close_array_element("value");
            }
            serializer.close_array("values");

        }
    }

    template<class Type>
    void apply_uniform_variant(uniform_variant const&, GLint, GLint)
    {
        static_assert(std::is_same_v<void, Type>, "Function should only be instantiated with void. Other instantiation are found in uniform_variant.inline");
    }

    #include "grender/uniform_variant.inline"

    template<class Type>
    uniform_variant::vtable get_uniform_vtable()
    {
        return uniform_variant::vtable{
            &construct_uniform_variant<Type>,
            &destroy_uniform_variant<Type>,
            &copy_array_uniform_variant<Type>,
            &move_uniform_variant<Type>,
            &process_uniform_variant<Type>,
            &apply_uniform_variant<Type>,
        };
    }

    void uniform_variant::apply(GLint location, GLint texture_unit) const
    {
        get_vtable(m_type).m_apply(*this, location, texture_unit);
    }

    bool uniform_variant::is_texture() const
    {
        switch (m_type)
        {
        case type::sampler_1d:
        case type::sampler_1d_array:
        case type::sampler_1d_shadow:
        case type::sampler_1d_array_shadow:
        case type::sampler_2d:
        case type::sampler_2d_array:
        case type::sampler_2d_shadow:
        case type::sampler_2d_array_shadow:
        case type::sampler_3d:
        case type::sampler_cube:
        case type::sampler_cube_shadow:
            return true;
        }
        return false;
    }

    uniform_variant::vtable uniform_variant::get_vtable(type type)
    {
        switch (type)
        {
        case type::glbool:     return get_uniform_vtable<bool>();
        case type::glint:      return get_uniform_vtable<GLint>();
        case type::gluint:     return get_uniform_vtable<GLuint>();
        case type::glfloat:    return get_uniform_vtable<float>();
        case type::gldouble:   return get_uniform_vtable<double>();
        case type::bvec2:      return get_uniform_vtable<glm::bvec2>();
        case type::bvec3:      return get_uniform_vtable<glm::bvec3>();
        case type::bvec4:      return get_uniform_vtable<glm::bvec4>();
        case type::ivec2:      return get_uniform_vtable<glm::ivec2>();
        case type::ivec3:      return get_uniform_vtable<glm::ivec3>();
        case type::ivec4:      return get_uniform_vtable<glm::ivec4>();
        case type::uvec2:      return get_uniform_vtable<glm::uvec2>();
        case type::uvec3:      return get_uniform_vtable<glm::uvec3>();
        case type::uvec4:      return get_uniform_vtable<glm::uvec4>();
        case type::vec2:       return get_uniform_vtable<glm::vec2>();
        case type::vec3:       return get_uniform_vtable<glm::vec3>();
        case type::vec4:       return get_uniform_vtable<glm::vec4>();
        case type::dvec2:      return get_uniform_vtable<glm::dvec2>();
        case type::dvec3:      return get_uniform_vtable<glm::dvec3>();
        case type::dvec4:      return get_uniform_vtable<glm::dvec4>();
        case type::mat22:      return get_uniform_vtable<glm::mat2x2>();
        case type::mat23:      return get_uniform_vtable<glm::mat2x3>();
        case type::mat24:      return get_uniform_vtable<glm::mat2x4>();
        case type::mat32:      return get_uniform_vtable<glm::mat3x2>();
        case type::mat33:      return get_uniform_vtable<glm::mat3x3>();
        case type::mat34:      return get_uniform_vtable<glm::mat3x4>();
        case type::mat42:      return get_uniform_vtable<glm::mat4x2>();
        case type::mat43:      return get_uniform_vtable<glm::mat4x3>();
        case type::mat44:      return get_uniform_vtable<glm::mat4x4>();
        case type::sampler_1d:
        case type::sampler_1d_array:
        case type::sampler_1d_shadow:
        case type::sampler_1d_array_shadow:
        case type::sampler_2d:
        case type::sampler_2d_array:
        case type::sampler_2d_shadow:
        case type::sampler_2d_array_shadow:
        case type::sampler_3d:
        case type::sampler_cube:
        case type::sampler_cube_shadow:
            return get_uniform_vtable<gcore::resource_handle<texture>>();
        case type::invalid:     
            return get_uniform_vtable<void>();
        }
        return get_uniform_vtable<void>();
    }

    GLint uniform_variant::get_gl_texture_type(type type)
    {
        switch (type)
        {
        case uniform_variant::type::sampler_1d: return GL_TEXTURE_1D;
        case uniform_variant::type::sampler_1d_array: return GL_TEXTURE_1D_ARRAY;
        case uniform_variant::type::sampler_1d_shadow: return GL_TEXTURE_1D;
        case uniform_variant::type::sampler_1d_array_shadow: return GL_TEXTURE_1D_ARRAY;
        case uniform_variant::type::sampler_2d: return GL_TEXTURE_2D;
        case uniform_variant::type::sampler_2d_array: return GL_TEXTURE_2D_ARRAY;
        case uniform_variant::type::sampler_2d_shadow: return GL_TEXTURE_2D;
        case uniform_variant::type::sampler_2d_array_shadow: return GL_TEXTURE_2D_ARRAY;
        case uniform_variant::type::sampler_3d: return GL_TEXTURE_3D;
        case uniform_variant::type::sampler_cube: return GL_TEXTURE_CUBE_MAP;
        case uniform_variant::type::sampler_cube_shadow: return GL_TEXTURE_CUBE_MAP;
        }
        assert(false);
        return GL_SAMPLER_1D;
    }

    uniform_variant::type get_type_from_gl_enum(GLenum enum_type)
    {
        switch (enum_type)
        {
        case GL_BOOL:                       return uniform_variant::type::glbool;
        case GL_INT:                        return uniform_variant::type::glint;
        case GL_UNSIGNED_INT:               return uniform_variant::type::gluint;
        case GL_FLOAT:                      return uniform_variant::type::glfloat;
        case GL_DOUBLE:                     return uniform_variant::type::gldouble;
        case GL_BOOL_VEC2:                  return uniform_variant::type::bvec2;
        case GL_BOOL_VEC3:                  return uniform_variant::type::bvec3;
        case GL_BOOL_VEC4:                  return uniform_variant::type::bvec4;
        case GL_INT_VEC2:                   return uniform_variant::type::ivec2;
        case GL_INT_VEC3:                   return uniform_variant::type::ivec3;
        case GL_INT_VEC4:                   return uniform_variant::type::ivec4;
        case GL_UNSIGNED_INT_VEC2:          return uniform_variant::type::uvec2;
        case GL_UNSIGNED_INT_VEC3:          return uniform_variant::type::uvec3;
        case GL_UNSIGNED_INT_VEC4:          return uniform_variant::type::uvec4;
        case GL_FLOAT_VEC2:                 return uniform_variant::type::vec2;
        case GL_FLOAT_VEC3:                 return uniform_variant::type::vec3;
        case GL_FLOAT_VEC4:                 return uniform_variant::type::vec4;
        case GL_DOUBLE_VEC2:                return uniform_variant::type::dvec2;
        case GL_DOUBLE_VEC3:                return uniform_variant::type::dvec3;
        case GL_DOUBLE_VEC4:                return uniform_variant::type::dvec4;
        case GL_FLOAT_MAT2:                 return uniform_variant::type::mat22;
        case GL_FLOAT_MAT2x3:               return uniform_variant::type::mat23;
        case GL_FLOAT_MAT2x4:               return uniform_variant::type::mat24;
        case GL_FLOAT_MAT3x2:               return uniform_variant::type::mat32;
        case GL_FLOAT_MAT3:                 return uniform_variant::type::mat33;
        case GL_FLOAT_MAT3x4:               return uniform_variant::type::mat34;
        case GL_FLOAT_MAT4x2:               return uniform_variant::type::mat42;
        case GL_FLOAT_MAT4x3:               return uniform_variant::type::mat43;
        case GL_FLOAT_MAT4:                 return uniform_variant::type::mat44;
        case GL_SAMPLER_1D:                 return uniform_variant::type::sampler_1d;
        case GL_SAMPLER_1D_ARRAY:           return uniform_variant::type::sampler_1d_array;
        case GL_SAMPLER_1D_SHADOW:          return uniform_variant::type::sampler_1d_shadow;
        case GL_SAMPLER_1D_ARRAY_SHADOW:    return uniform_variant::type::sampler_1d_array_shadow;
        case GL_SAMPLER_2D:                 return uniform_variant::type::sampler_2d;
        case GL_SAMPLER_2D_ARRAY:           return uniform_variant::type::sampler_2d_array;
        case GL_SAMPLER_2D_SHADOW:          return uniform_variant::type::sampler_2d_shadow;
        case GL_SAMPLER_2D_ARRAY_SHADOW:    return uniform_variant::type::sampler_2d_array_shadow;
        case GL_SAMPLER_3D:                 return uniform_variant::type::sampler_3d;
        case GL_SAMPLER_CUBE:               return uniform_variant::type::sampler_cube;
        case GL_SAMPLER_CUBE_SHADOW:        return uniform_variant::type::sampler_cube_shadow;
        default:                            return uniform_variant::type::invalid;
        }
        
    }
}

