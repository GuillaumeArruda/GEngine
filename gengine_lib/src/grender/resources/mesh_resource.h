#pragma once

#include <array>
#include <filesystem>

#include <GL/glew.h>

#include "gtl/span.h"

#include "gmath/axis_aligned_box.h"

#include "gcore/resource.h"
#include "gcore/spaces.h"



struct aiMesh;

namespace grender
{
    struct mesh_serialization_info
    {
        void process(gserializer::serializer& serializer);
        std::size_t m_version = 1;
        std::vector<glm::vec3> m_positions;
        std::vector<glm::vec3> m_normals;
        std::vector<glm::vec3> m_tangents;
        std::vector<glm::vec3> m_bitangents;
        std::vector<glm::vec2> m_uv_coords;
        std::vector<glm::ivec3> m_faces;
        glm::vec3 m_min_extent;
        glm::vec3 m_max_extent;
    };

    struct mesh_resource : gcore::resource
    {
        void process(gserializer::serializer& serializer) override;
        void draw();

        gmath::axis_aligned_box<gcore::model_space> const& get_extent() const { return m_extent; }

    private:
        enum vbo_type
        {
            position,
            normal,
            uv,
            tangent,
            bitangent,
            faces,
            count
        };

        bool do_load_async() override;
        bool do_load_sync() override;
        void do_unload() override;

        GLuint m_vao = 0;
        std::array<GLuint, vbo_type::count> m_vbo = { 0 };
        GLuint m_number_of_element = 0;
        gmath::axis_aligned_box<gcore::model_space> m_extent;
        std::filesystem::path m_bin_filepath;
        GSERIALIZER_DECLARE_SUBCLASS_FACTORY_REGISTRATION();
    };
}