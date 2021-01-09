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
        GSERIALIZER_ADD_ENUM_ELEMENT(uniform_variant::type::sampler_2d)
        GSERIALIZER_ADD_ENUM_ELEMENT(uniform_variant::type::sampler_3d)
        GSERIALIZER_ADD_ENUM_ELEMENT(uniform_variant::type::sampler_cube)
    GSERIALIZER_END_ENUM_HANDLER(uniform_variant::type)

    uniform_variant::uniform_variant(type type)
        : m_type(type)
    {
        default_construct();
    }
    uniform_variant::uniform_variant(uniform_variant const& copy_value) noexcept
        : m_type(copy_value.m_type)
    {
        copy(copy_value);
    }

    uniform_variant& uniform_variant::operator=(uniform_variant const& copy_value) noexcept
    {
        if (this == &copy_value)
            return *this;

        m_type = copy_value.m_type;
        copy(copy_value);
        return *this;
    }

    uniform_variant::uniform_variant(uniform_variant&& move_value) noexcept
        : m_type(move_value.m_type)
    {
        move(std::move(move_value));
        move_value.m_type = type::invalid;
    }

    uniform_variant& uniform_variant::operator=(uniform_variant&& move_value) noexcept
    {
        if (this == &move_value)
            return *this;

        destroy();
        m_type = move_value.m_type;
        move(std::move(move_value));
        move_value.m_type = type::invalid;
        return *this;
    }

    uniform_variant::~uniform_variant()
    {
        destroy();
    }

    void uniform_variant::apply(GLint location) const
    {
        switch (m_type)
        {
        case type::glbool:      gl_exec(glUniform1i, location, m_bool); break;
        case type::glint:       gl_exec(glUniform1i, location, m_int); break;
        case type::gluint:      gl_exec(glUniform1ui, location, m_uint); break;
        case type::glfloat:     gl_exec(glUniform1f, location, m_float); break;
        case type::gldouble:    gl_exec(glUniform1d, location, m_double); break;
        case type::bvec2:       gl_exec(glUniform2i, location, static_cast<GLint>(m_bvec2[0]), static_cast<GLint>(m_bvec2[1])); break;
        case type::bvec3:       gl_exec(glUniform3i, location, static_cast<GLint>(m_bvec3[0]), static_cast<GLint>(m_bvec3[1]), static_cast<GLint>(m_bvec3[2])); break;
        case type::bvec4:       gl_exec(glUniform4i, location, static_cast<GLint>(m_bvec4[0]), static_cast<GLint>(m_bvec4[1]), static_cast<GLint>(m_bvec4[2]), static_cast<GLint>(m_bvec4[3])); break;
        case type::ivec2:       gl_exec(glUniform2iv, location, 1, glm::value_ptr(m_ivec2)); break;
        case type::ivec3:       gl_exec(glUniform3iv, location, 1, glm::value_ptr(m_ivec3)); break;
        case type::ivec4:       gl_exec(glUniform4iv, location, 1, glm::value_ptr(m_ivec4)); break;
        case type::uvec2:       gl_exec(glUniform2uiv, location, 1, glm::value_ptr(m_uvec2)); break;
        case type::uvec3:       gl_exec(glUniform3uiv, location, 1, glm::value_ptr(m_uvec3)); break;
        case type::uvec4:       gl_exec(glUniform4uiv, location, 1, glm::value_ptr(m_uvec4)); break;
        case type::vec2:        gl_exec(glUniform2fv, location, 1, glm::value_ptr(m_vec2)); break;
        case type::vec3:        gl_exec(glUniform3fv, location, 1, glm::value_ptr(m_vec3)); break;
        case type::vec4:        gl_exec(glUniform4fv, location, 1, glm::value_ptr(m_vec4)); break;
        case type::dvec2:       gl_exec(glUniform2dv, location, 1, glm::value_ptr(m_dvec2)); break;
        case type::dvec3:       gl_exec(glUniform3dv, location, 1, glm::value_ptr(m_dvec3)); break;
        case type::dvec4:       gl_exec(glUniform4dv, location, 1, glm::value_ptr(m_dvec4)); break;
        case type::mat22:       gl_exec(glUniformMatrix2fv,     location, 1, static_cast<GLboolean>(GL_FALSE), glm::value_ptr(m_mat22)); break;
        case type::mat23:       gl_exec(glUniformMatrix2x3fv,   location, 1, static_cast<GLboolean>(GL_FALSE), glm::value_ptr(m_mat23)); break;
        case type::mat24:       gl_exec(glUniformMatrix2x4fv,   location, 1, static_cast<GLboolean>(GL_FALSE), glm::value_ptr(m_mat24)); break;
        case type::mat32:       gl_exec(glUniformMatrix3x2fv,   location, 1, static_cast<GLboolean>(GL_FALSE), glm::value_ptr(m_mat32)); break;
        case type::mat33:       gl_exec(glUniformMatrix3fv,     location, 1, static_cast<GLboolean>(GL_FALSE), glm::value_ptr(m_mat33)); break;
        case type::mat34:       gl_exec(glUniformMatrix3x4fv,   location, 1, static_cast<GLboolean>(GL_FALSE), glm::value_ptr(m_mat34)); break;
        case type::mat42:       gl_exec(glUniformMatrix4x2fv,   location, 1, static_cast<GLboolean>(GL_FALSE), glm::value_ptr(m_mat42)); break;
        case type::mat43:       gl_exec(glUniformMatrix4x3fv,   location, 1, static_cast<GLboolean>(GL_FALSE), glm::value_ptr(m_mat43)); break;
        case type::mat44:       gl_exec(glUniformMatrix4fv,     location, 1, static_cast<GLboolean>(GL_FALSE), glm::value_ptr(m_mat44)); break;
        case type::sampler_1d:
        case type::sampler_2d:
        case type::sampler_3d:
        case type::sampler_cube:
        {
            if (m_texture_info.m_texture_id == 0)
            {
                if (m_texture_info.m_texture.is_loaded())
                {
                    gl_exec(glActiveTexture, GL_TEXTURE0 + location);
                    gl_exec(glBindTexture, get_gl_texture_type(), m_texture_info.m_texture->get_id());
                    gl_exec(glUniform1i, location, location);
                }
                else
                {
                    gl_exec(glActiveTexture, GL_TEXTURE0 + location);
                    gl_exec(glBindTexture, get_gl_texture_type(), 0);
                    gl_exec(glUniform1i, location, location);
                }
            }
            else
            {
                gl_exec(glActiveTexture, GL_TEXTURE0 + location);
                gl_exec(glBindTexture, get_gl_texture_type(), m_texture_info.m_texture_id);
                gl_exec(glUniform1i, location, location);
            }
            break;
        }
        case type::invalid:     break;
        }
    }

    void uniform_variant::process(gserializer::serializer& serializer)
    {
        using gserializer::process;
        type const old_type = m_type;
        serializer.process("type", m_type);
        if (old_type != m_type)
        {
            destroy();
            default_construct();
        }
        switch (m_type)
        {
        case type::glbool:      process(serializer, m_bool);    break;
        case type::glint:       process(serializer, m_int);     break;
        case type::gluint:      process(serializer, m_uint);    break;
        case type::glfloat:     process(serializer, m_float);   break;
        case type::gldouble:    process(serializer, m_double);  break;
        case type::bvec2:       process(serializer, m_bvec2);   break;
        case type::bvec3:       process(serializer, m_bvec3);   break;
        case type::bvec4:       process(serializer, m_bvec4);   break;
        case type::ivec2:       process(serializer, m_ivec2);   break;
        case type::ivec3:       process(serializer, m_ivec3);   break;
        case type::ivec4:       process(serializer, m_ivec4);   break;
        case type::uvec2:       process(serializer, m_uvec2);   break;
        case type::uvec3:       process(serializer, m_uvec3);   break;
        case type::uvec4:       process(serializer, m_uvec4);   break;
        case type::vec2:        process(serializer, m_vec2);    break;
        case type::vec3:        process(serializer, m_vec3);    break;
        case type::vec4:        process(serializer, m_vec4);    break;
        case type::dvec2:       process(serializer, m_dvec2);   break;
        case type::dvec3:       process(serializer, m_dvec3);   break;
        case type::dvec4:       process(serializer, m_dvec4);   break;
        case type::mat22:       process(serializer, m_mat22);   break;
        case type::mat23:       process(serializer, m_mat23);   break;
        case type::mat24:       process(serializer, m_mat24);   break;
        case type::mat32:       process(serializer, m_mat32);   break;
        case type::mat33:       process(serializer, m_mat33);   break;
        case type::mat34:       process(serializer, m_mat34);   break;
        case type::mat42:       process(serializer, m_mat42);   break;
        case type::mat43:       process(serializer, m_mat43);   break;
        case type::mat44:       process(serializer, m_mat44);   break;
        case type::sampler_1d:
        case type::sampler_2d:
        case type::sampler_3d:
        case type::sampler_cube:
            process(serializer, m_texture_info.m_texture_uuid, m_texture_info.m_texture); break;
        }
    }

    void uniform_variant::default_construct()
    {
        switch (m_type)
        {
        case type::glbool:      new((void*)&m_bool)    bool{}; break;
        case type::glint:       new((void*)&m_int)     GLint{}; break;
        case type::gluint:      new((void*)&m_uint)    GLuint{}; break;
        case type::glfloat:     new((void*)&m_float)   float{}; break;
        case type::gldouble:    new((void*)&m_double)  double{}; break;
        case type::bvec2:       new((void*)&m_bvec2)   glm::bvec2(); break;
        case type::bvec3:       new((void*)&m_bvec3)   glm::bvec3(); break;
        case type::bvec4:       new((void*)&m_bvec4)   glm::bvec4(); break;
        case type::ivec2:       new((void*)&m_ivec2)   glm::ivec2(); break;
        case type::ivec3:       new((void*)&m_ivec3)   glm::ivec3(); break;
        case type::ivec4:       new((void*)&m_ivec4)   glm::ivec4(); break;
        case type::uvec2:       new((void*)&m_uvec2)   glm::uvec2(); break;
        case type::uvec3:       new((void*)&m_uvec3)   glm::uvec3(); break;
        case type::uvec4:       new((void*)&m_uvec4)   glm::uvec4(); break;
        case type::vec2:        new((void*)&m_vec2)    glm::vec2(); break;
        case type::vec3:        new((void*)&m_vec3)    glm::vec3(); break;
        case type::vec4:        new((void*)&m_vec4)    glm::vec4(); break;
        case type::dvec2:       new((void*)&m_dvec2)   glm::dvec2(); break;
        case type::dvec3:       new((void*)&m_dvec3)   glm::dvec3(); break;
        case type::dvec4:       new((void*)&m_dvec4)   glm::dvec4(); break;
        case type::mat22:       new((void*)&m_mat22)   glm::mat2x2(); break;
        case type::mat23:       new((void*)&m_mat23)   glm::mat2x3(); break;
        case type::mat24:       new((void*)&m_mat24)   glm::mat2x4(); break;
        case type::mat32:       new((void*)&m_mat32)   glm::mat3x2(); break;
        case type::mat33:       new((void*)&m_mat33)   glm::mat3x3(); break;
        case type::mat34:       new((void*)&m_mat34)   glm::mat3x4(); break;
        case type::mat42:       new((void*)&m_mat42)   glm::mat4x2(); break;
        case type::mat43:       new((void*)&m_mat43)   glm::mat4x3(); break;
        case type::mat44:       new((void*)&m_mat44)   glm::mat4x4(); break;
        case type::sampler_1d:
        case type::sampler_2d:
        case type::sampler_3d:
        case type::sampler_cube: new((void*)&m_texture_info) texture_info(); break;
        case type::invalid:     break;
        }
    }

    void uniform_variant::destroy()
    {
        switch (m_type)
        {
        case type::sampler_1d:
        case type::sampler_2d:
        case type::sampler_3d:
        case type::sampler_cube: 
            m_texture_info.~texture_info();
            break;
        }
    }

    void uniform_variant::move(uniform_variant&& move)
    {
        switch (m_type)
        {
        case type::glbool:      new((void*)&m_bool)    bool(std::move(move.m_bool)); break;
        case type::glint:       new((void*)&m_int)     GLint(std::move(move.m_int)); break;
        case type::gluint:      new((void*)&m_uint)    GLuint(std::move(move.m_uint)); break;
        case type::glfloat:     new((void*)&m_float)   float(std::move(move.m_float)); break;
        case type::gldouble:    new((void*)&m_double)  double(std::move(move.m_double)); break;
        case type::bvec2:       new((void*)&m_bvec2)   glm::bvec2(std::move(move.m_bvec2)); break;
        case type::bvec3:       new((void*)&m_bvec3)   glm::bvec3(std::move(move.m_bvec3)); break;
        case type::bvec4:       new((void*)&m_bvec4)   glm::bvec4(std::move(move.m_bvec4)); break;
        case type::ivec2:       new((void*)&m_ivec2)   glm::ivec2(std::move(move.m_ivec2)); break;
        case type::ivec3:       new((void*)&m_ivec3)   glm::ivec3(std::move(move.m_ivec3)); break;
        case type::ivec4:       new((void*)&m_ivec4)   glm::ivec4(std::move(move.m_ivec4)); break;
        case type::uvec2:       new((void*)&m_uvec2)   glm::uvec2(std::move(move.m_uvec2)); break;
        case type::uvec3:       new((void*)&m_uvec3)   glm::uvec3(std::move(move.m_uvec3)); break;
        case type::uvec4:       new((void*)&m_uvec4)   glm::uvec4(std::move(move.m_uvec4)); break;
        case type::vec2:        new((void*)&m_vec2)    glm::vec2(std::move(move.m_vec2)); break;
        case type::vec3:        new((void*)&m_vec3)    glm::vec3(std::move(move.m_vec3)); break;
        case type::vec4:        new((void*)&m_vec4)    glm::vec4(std::move(move.m_vec4)); break;
        case type::dvec2:       new((void*)&m_dvec2)   glm::dvec2(std::move(move.m_dvec2)); break;
        case type::dvec3:       new((void*)&m_dvec3)   glm::dvec3(std::move(move.m_dvec3)); break;
        case type::dvec4:       new((void*)&m_dvec4)   glm::dvec4(std::move(move.m_dvec4)); break;
        case type::mat23:       new((void*)&m_mat23)   glm::mat2x3(std::move(move.m_mat23)); break;
        case type::mat22:       new((void*)&m_mat22)   glm::mat2x2(std::move(move.m_mat22)); break;
        case type::mat24:       new((void*)&m_mat24)   glm::mat2x4(std::move(move.m_mat24)); break;
        case type::mat32:       new((void*)&m_mat32)   glm::mat3x2(std::move(move.m_mat32)); break;
        case type::mat33:       new((void*)&m_mat33)   glm::mat3x3(std::move(move.m_mat33)); break;
        case type::mat34:       new((void*)&m_mat34)   glm::mat3x4(std::move(move.m_mat34)); break;
        case type::mat42:       new((void*)&m_mat42)   glm::mat4x2(std::move(move.m_mat42)); break;
        case type::mat43:       new((void*)&m_mat43)   glm::mat4x3(std::move(move.m_mat43)); break;
        case type::mat44:       new((void*)&m_mat44)   glm::mat4x4(std::move(move.m_mat44)); break;
        case type::sampler_1d:
        case type::sampler_2d:
        case type::sampler_3d:
        case type::sampler_cube:
            new((void*)&m_texture_info) texture_info(std::move(move.m_texture_info)); break;
        case type::invalid:     break;
        }
    }

    void uniform_variant::copy(uniform_variant const& copy)
    {
        switch (m_type)
        {
        case type::glbool:      new((void*)&m_bool)    bool(copy.m_bool); break;
        case type::glint:       new((void*)&m_int)     GLint(copy.m_int); break;
        case type::gluint:      new((void*)&m_uint)    GLuint(copy.m_uint); break;
        case type::glfloat:     new((void*)&m_float)   float(copy.m_float); break;
        case type::gldouble:    new((void*)&m_double)  double(copy.m_double); break;
        case type::bvec2:       new((void*)&m_bvec2)   glm::bvec2(copy.m_bvec2); break;
        case type::bvec3:       new((void*)&m_bvec3)   glm::bvec3(copy.m_bvec3); break;
        case type::bvec4:       new((void*)&m_bvec4)   glm::bvec4(copy.m_bvec4); break;
        case type::ivec2:       new((void*)&m_ivec2)   glm::ivec2(copy.m_ivec2); break;
        case type::ivec3:       new((void*)&m_ivec3)   glm::ivec3(copy.m_ivec3); break;
        case type::ivec4:       new((void*)&m_ivec4)   glm::ivec4(copy.m_ivec4); break;
        case type::uvec2:       new((void*)&m_uvec2)   glm::uvec2(copy.m_uvec2); break;
        case type::uvec3:       new((void*)&m_uvec3)   glm::uvec3(copy.m_uvec3); break;
        case type::uvec4:       new((void*)&m_uvec4)   glm::uvec4(copy.m_uvec4); break;
        case type::vec2:        new((void*)&m_vec2)    glm::vec2(copy.m_vec2); break;
        case type::vec3:        new((void*)&m_vec3)    glm::vec3(copy.m_vec3); break;
        case type::vec4:        new((void*)&m_vec4)    glm::vec4(copy.m_vec4); break;
        case type::dvec2:       new((void*)&m_dvec2)   glm::dvec2(copy.m_dvec2); break;
        case type::dvec3:       new((void*)&m_dvec3)   glm::dvec3(copy.m_dvec3); break;
        case type::dvec4:       new((void*)&m_dvec4)   glm::dvec4(copy.m_dvec4); break;
        case type::mat22:       new((void*)&m_mat22)   glm::mat2x2(copy.m_mat22); break;
        case type::mat23:       new((void*)&m_mat23)   glm::mat2x3(copy.m_mat23); break;
        case type::mat24:       new((void*)&m_mat24)   glm::mat2x4(copy.m_mat24); break;
        case type::mat32:       new((void*)&m_mat32)   glm::mat3x2(copy.m_mat32); break;
        case type::mat33:       new((void*)&m_mat33)   glm::mat3x3(copy.m_mat33); break;
        case type::mat34:       new((void*)&m_mat34)   glm::mat3x4(copy.m_mat34); break;
        case type::mat42:       new((void*)&m_mat42)   glm::mat4x2(copy.m_mat42); break;
        case type::mat43:       new((void*)&m_mat43)   glm::mat4x3(copy.m_mat43); break;
        case type::mat44:       new((void*)&m_mat44)   glm::mat4x4(copy.m_mat44); break;
        case type::sampler_1d:
        case type::sampler_2d:
        case type::sampler_3d:
        case type::sampler_cube:
            new((void*)&m_texture_info) texture_info(copy.m_texture_info); break;
        case type::invalid:     break;
        }
    }
    GLint uniform_variant::get_gl_texture_type() const
    {
        switch (m_type)
        {
        case uniform_variant::type::sampler_1d: return GL_TEXTURE_1D;
        case uniform_variant::type::sampler_2d: return GL_TEXTURE_2D;
        case uniform_variant::type::sampler_3d: return GL_TEXTURE_3D;
        case uniform_variant::type::sampler_cube: return GL_TEXTURE_CUBE_MAP;
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
        case GL_SAMPLER_2D:                 return uniform_variant::type::sampler_2d;
        case GL_SAMPLER_3D:                 return uniform_variant::type::sampler_3d;
        case GL_SAMPLER_CUBE:               return uniform_variant::type::sampler_cube;
        default:                            return uniform_variant::type::invalid;
        }
        
    }
}

