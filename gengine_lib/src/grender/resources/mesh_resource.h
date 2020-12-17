#pragma once

#include <array>

#include <GL/glew.h>

#include "gtl/span.h"
#include "gcore/resource.h"


struct aiMesh;

namespace grender
{
    struct mesh
    {
        mesh(aiMesh const& mesh);
        ~mesh();

        mesh(mesh&&) noexcept;
        mesh& operator=(mesh&&) noexcept;

        mesh(mesh const&) = delete;
        mesh& operator=(mesh const&) = delete;

        void draw() const;

    private:
        enum vbo_type
        {
            position,
            normal,
            uv,
            faces,
            count
        };
        GLuint m_vao = 0;
        std::array<GLuint, vbo_type::count> m_vbo = {0};
        GLuint m_number_of_element = 0;
    };

    struct mesh_resource : gcore::resource
    {
        void process(gserializer::serializer& serializer) override;
        void load() override;
        void set_fbx_path(std::string path);

        gtl::span<mesh const> get_meshes() const { return m_submeshes; }

    private:
        std::vector<mesh> m_submeshes;
        std::string m_fbx_path;
        GSERIALIZER_DECLARE_SUBCLASS_FACTORY_REGISTRATION();
    };
}