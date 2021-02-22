#include "stdafx.h"

#include "grender/resources/mesh_resource.h"

#include "gserializer/gmath_serialization.h"
#include "gserializer/serializer.h"
#include "gserializer/serializers/binary_file_serializer.h"
#include "gserializer/enum.h"

#include "grender/utils.h"


GSERIALIZER_DEFINE_SUBCLASS_FACTORY_REGISTRATION(grender::mesh_resource);

namespace grender
{
    GSERIALIZER_BEGIN_ENUM_HANDLER(mesh_resource::primitive_type)
        GSERIALIZER_ADD_ENUM_ELEMENT(mesh_resource::primitive_type::triangle)
        GSERIALIZER_ADD_ENUM_ELEMENT(mesh_resource::primitive_type::patch)
    GSERIALIZER_END_ENUM_HANDLER(mesh_resource::primitive_type)

    void mesh_resource::process(gserializer::serializer& serializer)
    {
        resource::process(serializer);
        serializer.process("bin_file_path", m_bin_filepath);
        serializer.process("primitive_type", m_type);
    }

    void mesh_resource::draw() const
    {
        gl_exec(glBindVertexArray, m_vao);
        if (m_type == primitive_type::triangle)
        {
            gl_exec(glDrawElements, GL_TRIANGLES, m_number_of_element, GL_UNSIGNED_INT, nullptr);
        }
        else
        {
            gl_exec(glPatchParameteri, GL_PATCH_VERTICES, 3);
            gl_exec(glDrawElements, GL_PATCHES, m_number_of_element, GL_UNSIGNED_INT, nullptr);
        }
        gl_exec(glBindVertexArray, 0);
    }

    bool mesh_resource::do_load_async()
    {
        gserializer::binary_file_reader reader(m_bin_filepath.string().c_str());
        mesh_serialization_info info;
        reader.process("mesh_info", info);

        std::size_t const number_of_vertices = static_cast<GLuint>(info.m_positions.size());
        if (number_of_vertices == 0)
            return false;

        m_extent = gmath::axis_aligned_box<gcore::model_space>{ gmath::position<gcore::model_space>(glm::vec4(info.m_min_extent, 1.f)), gmath::position<gcore::model_space>(glm::vec4(info.m_max_extent, 1.f)) };

        gl_exec(glGenBuffers, 1, &m_vbo[vbo_type::position]);
        gl_exec(glBindBuffer, GL_ARRAY_BUFFER, m_vbo[vbo_type::position]);
        gl_exec(glBufferData, GL_ARRAY_BUFFER, info.m_positions.size() * sizeof(glm::vec3), info.m_positions.data(), GL_STATIC_DRAW);


        if (info.m_normals.size() == number_of_vertices)
        {
            gl_exec(glGenBuffers, 1, &m_vbo[vbo_type::normal]);
            gl_exec(glBindBuffer, GL_ARRAY_BUFFER, m_vbo[vbo_type::normal]);
            gl_exec(glBufferData, GL_ARRAY_BUFFER, info.m_normals.size() * sizeof(glm::vec3), info.m_normals.data(), GL_STATIC_DRAW);
        }

        if (info.m_uv_coords.size() == number_of_vertices)
        {
            gl_exec(glGenBuffers, 1, &m_vbo[vbo_type::uv]);
            gl_exec(glBindBuffer, GL_ARRAY_BUFFER, m_vbo[vbo_type::uv]);
            gl_exec(glBufferData, GL_ARRAY_BUFFER, info.m_uv_coords.size() * sizeof(glm::vec2), info.m_uv_coords.data(), GL_STATIC_DRAW);
        }

        if (info.m_tangents.size() == number_of_vertices && info.m_bitangents.size() == number_of_vertices)
        {
            gl_exec(glGenBuffers, 2, &m_vbo[vbo_type::tangent]);
            gl_exec(glBindBuffer, GL_ARRAY_BUFFER, m_vbo[vbo_type::tangent]);
            gl_exec(glBufferData, GL_ARRAY_BUFFER, info.m_tangents.size() * sizeof(glm::vec3), info.m_tangents.data(), GL_STATIC_DRAW);
            gl_exec(glBindBuffer, GL_ARRAY_BUFFER, m_vbo[vbo_type::bitangent]);
            gl_exec(glBufferData, GL_ARRAY_BUFFER, info.m_tangents.size() * sizeof(glm::vec3), info.m_bitangents.data(), GL_STATIC_DRAW);
        }

        if (info.m_faces.size() != 0)
        {
            m_number_of_element = static_cast<GLuint>(info.m_faces.size()) * 3;
            gl_exec(glGenBuffers, 1, &m_vbo[vbo_type::faces]);
            gl_exec(glBindBuffer, GL_ELEMENT_ARRAY_BUFFER, m_vbo[vbo_type::faces]);
            gl_exec(glBufferData, GL_ELEMENT_ARRAY_BUFFER, info.m_faces.size() * sizeof(glm::ivec3), info.m_faces.data(), GL_STATIC_DRAW);
        }
        else
        {
            m_number_of_element = static_cast<GLuint>(number_of_vertices);
        }

        return true;
    }
    
    bool mesh_resource::do_load_sync()
    {
        gl_exec(glGenVertexArrays, 1, &m_vao);
        gl_exec(glBindVertexArray, m_vao);

        if (m_vbo[vbo_type::position] != 0)
        {
            gl_exec(glBindBuffer, GL_ARRAY_BUFFER, m_vbo[vbo_type::position]);
            gl_exec(glVertexAttribPointer, vbo_type::position, 3, GL_FLOAT, static_cast<GLboolean>(GL_FALSE), 0, nullptr);
            gl_exec(glEnableVertexAttribArray, vbo_type::position);
        }

        if (m_vbo[vbo_type::normal] != 0)
        {
            gl_exec(glBindBuffer, GL_ARRAY_BUFFER, m_vbo[vbo_type::normal]);
            gl_exec(glVertexAttribPointer, vbo_type::normal, 3, GL_FLOAT, static_cast<GLboolean>(GL_FALSE), 0, nullptr);
            gl_exec(glEnableVertexAttribArray, vbo_type::normal);
        }

        if (m_vbo[vbo_type::uv] != 0)
        {
            gl_exec(glBindBuffer, GL_ARRAY_BUFFER, m_vbo[vbo_type::uv]);
            gl_exec(glVertexAttribPointer, vbo_type::uv, 2, GL_FLOAT, static_cast<GLboolean>(GL_FALSE), 0, nullptr);
            gl_exec(glEnableVertexAttribArray, vbo_type::uv);
        }

        if (m_vbo[vbo_type::tangent] != 0)
        {
            gl_exec(glBindBuffer, GL_ARRAY_BUFFER, m_vbo[vbo_type::tangent]);
            gl_exec(glVertexAttribPointer, vbo_type::tangent, 3, GL_FLOAT, static_cast<GLboolean>(GL_FALSE), 0, nullptr);
            gl_exec(glEnableVertexAttribArray, vbo_type::tangent);
        }

        if (m_vbo[vbo_type::bitangent] != 0)
        {
            gl_exec(glBindBuffer, GL_ARRAY_BUFFER, m_vbo[vbo_type::bitangent]);
            gl_exec(glVertexAttribPointer, vbo_type::bitangent, 3, GL_FLOAT, static_cast<GLboolean>(GL_FALSE), 0, nullptr);
            gl_exec(glEnableVertexAttribArray, vbo_type::bitangent);
        }

        if (m_vbo[vbo_type::faces] - 1 != 0)
        {
            gl_exec(glBindBuffer, GL_ELEMENT_ARRAY_BUFFER, m_vbo[vbo_type::faces]);
        }

        gl_exec(glBindBuffer, GL_ARRAY_BUFFER, 0);
        gl_exec(glBindVertexArray, 0);
        return true;
    }

    void mesh_resource::do_unload()
    {
        gl_exec(glDeleteBuffers, vbo_type::count, m_vbo.data());
        gl_exec(glDeleteVertexArrays, 1, &m_vao);
    }

    void mesh_serialization_info::process(gserializer::serializer& serializer)
    {
        serializer.process("version", m_version);
        serializer.process("positions", m_positions, "position");
        serializer.process("normals", m_normals, "normal");
        serializer.process("tangents", m_tangents, "tangent");
        serializer.process("bitangents", m_bitangents, "bitangent");
        serializer.process("uv_coords", m_uv_coords, "uv_coord");
        serializer.process("faces", m_faces, "face");
        serializer.process("min_extent", m_min_extent);
        serializer.process("max_extent", m_max_extent);
    }
}

