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
    struct mesh
    {
        mesh(aiMesh const& mesh, gmath::axis_aligned_box<gcore::model_space>& out_extent);
        ~mesh();

        mesh(mesh&&) noexcept;
        mesh& operator=(mesh&&) noexcept;

        mesh(mesh const&) = delete;
        mesh& operator=(mesh const&) = delete;

        void draw() const;
        void bind();
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
        GLuint m_vao = 0;
        std::array<GLuint, vbo_type::count> m_vbo = {0};
        GLuint m_number_of_element = 0;
    };

    struct mesh_resource : gcore::resource
    {
        void process(gserializer::serializer& serializer) override;
        bool need_async_load() const override { return true; };

        gtl::span<mesh const> get_meshes() const { return m_submeshes; }
        gmath::axis_aligned_box<gcore::model_space> const& get_extent() const { return m_extent; }

    private:
        bool do_load_async() override;
        bool do_load_sync() override;
        void do_unload() override;

        gmath::axis_aligned_box<gcore::model_space> m_extent;
        std::vector<mesh> m_submeshes;
        std::filesystem::path m_fbx_path;
        GSERIALIZER_DECLARE_SUBCLASS_FACTORY_REGISTRATION();
    };
}