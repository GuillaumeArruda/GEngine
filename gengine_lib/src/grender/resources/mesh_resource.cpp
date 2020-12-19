#include "stdafx.h"

#include "grender/resources/mesh_resource.h"

#include "gserializer/serializer.h"

#include <assimp/Importer.hpp>
#include <assimp/mesh.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include "grender/utils.h"


GSERIALIZER_DEFINE_SUBCLASS_FACTORY_REGISTRATION(grender::mesh_resource);

namespace grender
{
    mesh::mesh(aiMesh const& mesh, gmath::axis_aligned_box<gcore::model_space>& out_extent)
        : m_number_of_element(mesh.mNumVertices)
    {
        gl_exec(glGenVertexArrays, 1, &m_vao);
        gl_exec(glBindVertexArray, m_vao);

        if (mesh.HasPositions())
        {
            gl_exec(glGenBuffers, 1, &m_vbo[vbo_type::position]);
            gl_exec(glBindBuffer, GL_ARRAY_BUFFER, m_vbo[vbo_type::position]);
            gl_exec(glBufferData, GL_ARRAY_BUFFER, mesh.mNumVertices * sizeof(float) * 3ull, mesh.mVertices, GL_STATIC_DRAW);
            gl_exec(glVertexAttribPointer, vbo_type::position, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
            gl_exec(glEnableVertexAttribArray, vbo_type::position);

            glm::vec4 min(std::numeric_limits<float>::max());
            glm::vec4 max(std::numeric_limits<float>::lowest());
            for (std::uint32_t i = 0; i < mesh.mNumVertices; ++i)
            {
                min[0] = std::min(min[0], mesh.mVertices[i][0]);
                min[1] = std::min(min[1], mesh.mVertices[i][1]);
                min[2] = std::min(min[2], mesh.mVertices[i][2]);
                max[0] = std::max(max[0], mesh.mVertices[i][0]);
                max[1] = std::max(max[1], mesh.mVertices[i][1]);
                max[2] = std::max(max[2], mesh.mVertices[i][2]);
            }
            min[3] = 1.f;
            max[3] = 1.f;
            gmath::axis_aligned_box<gcore::model_space> const mesh_box{ gmath::position<gcore::model_space>(min), gmath::position<gcore::model_space>(max) };
            out_extent = out_extent.merge(mesh_box);
        }

        if (mesh.HasNormals())
        {
            gl_exec(glGenBuffers, 1, &m_vbo[vbo_type::normal]);
            gl_exec(glBindBuffer, GL_ARRAY_BUFFER, m_vbo[vbo_type::normal]);
            gl_exec(glBufferData, GL_ARRAY_BUFFER, mesh.mNumVertices * sizeof(float) * 3ull, mesh.mNormals, GL_STATIC_DRAW);
            gl_exec(glVertexAttribPointer, vbo_type::normal, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
            gl_exec(glEnableVertexAttribArray, vbo_type::normal);
        }

        if (mesh.HasTextureCoords(0))
        {
            std::vector<float> uv_cord;
            uv_cord.resize(mesh.mNumVertices * 2ull);
            for (std::size_t i = 0; i < mesh.mNumVertices; ++i)
            {
                uv_cord[i * 2 + 0] = mesh.mTextureCoords[0][i].x;
                uv_cord[i * 2 + 1] = mesh.mTextureCoords[0][i].y;
            }

            gl_exec(glGenBuffers, 1, &m_vbo[vbo_type::uv]);
            gl_exec(glBindBuffer, GL_ARRAY_BUFFER, m_vbo[vbo_type::uv]);
            gl_exec(glBufferData, GL_ARRAY_BUFFER, uv_cord.size() * sizeof(float), uv_cord.data(), GL_STATIC_DRAW);
            gl_exec(glBindBuffer, GL_ARRAY_BUFFER, m_vbo[2]);
            gl_exec(glVertexAttribPointer, vbo_type::uv, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
            gl_exec(glEnableVertexAttribArray, vbo_type::uv);
        }

        if (mesh.HasFaces())
        {
            std::vector<std::uint32_t> faces_idx;
            faces_idx.reserve(static_cast<std::size_t>(mesh.mNumFaces) * 3);
            for (unsigned int i = 0; i < mesh.mNumFaces; ++i)
            {
                for (unsigned int j = 0; j < mesh.mFaces[i].mNumIndices; ++j)
                {
                    faces_idx.push_back(mesh.mFaces[i].mIndices[j]);
                }
            }

            gl_exec(glGenBuffers, 1, &m_vbo[vbo_type::faces]);
            gl_exec(glBindBuffer, GL_ELEMENT_ARRAY_BUFFER, m_vbo[vbo_type::faces]);
            gl_exec(glBufferData, GL_ELEMENT_ARRAY_BUFFER, faces_idx.size() * sizeof(std::uint32_t), faces_idx.data(), GL_STATIC_DRAW);
            gl_exec(glBindBuffer, GL_ELEMENT_ARRAY_BUFFER, m_vbo[vbo_type::faces]);
        }

        gl_exec(glBindBuffer, GL_ARRAY_BUFFER, 0);
        gl_exec(glBindVertexArray, 0);
    }

    mesh::~mesh()
    {
        gl_exec(glDeleteBuffers, vbo_type::count, m_vbo.data());
        gl_exec(glDeleteVertexArrays, 1, &m_vao);
    }

    mesh::mesh(mesh&& move) noexcept
        : m_vao(move.m_vao)
        , m_vbo{ move.m_vbo }
        , m_number_of_element(move.m_number_of_element)
    {
        move.m_vao = 0;
        move.m_vbo = std::array<GLuint, vbo_type::count>{ 0 };
        move.m_number_of_element = 0;
    }

    mesh& mesh::operator=(mesh&& move) noexcept
    {
        if (this == &move)
            return *this;

        gl_exec(glDeleteBuffers, vbo_type::count, m_vbo.data());
        gl_exec(glDeleteVertexArrays, 1, &m_vao);

        m_vao = move.m_vao;
        m_vbo = move.m_vbo;
        m_number_of_element = move.m_number_of_element;

        move.m_vao = 0;
        move.m_vbo = { 0 };
        move.m_number_of_element = 0;
        return *this;
    }

    void mesh::draw() const
    {
        gl_exec(glBindVertexArray, m_vao);
        gl_exec(glDrawElements, GL_TRIANGLES, m_number_of_element, GL_UNSIGNED_INT, nullptr);
        gl_exec(glBindVertexArray, 0);
    }

    void mesh_resource::process(gserializer::serializer& serializer)
    {
        resource::process(serializer);
        serializer.process("fbx_path", m_fbx_path);
    }

    void mesh_resource::load()
    {
        Assimp::Importer importer;
        aiScene const* scene = importer.ReadFile(m_fbx_path.c_str(),
            aiProcess_Triangulate |
            aiProcess_ImproveCacheLocality |
            aiProcess_FixInfacingNormals |
            aiProcess_OptimizeGraph |
            aiProcess_OptimizeMeshes);

        if (!scene)
        {
            std::cerr << importer.GetErrorString() << "\n";
            return;
        }

        m_submeshes.reserve(scene->mNumMeshes);
        for (unsigned int i = 0; i < scene->mNumMeshes; ++i)
        {
            m_submeshes.emplace_back(*scene->mMeshes[i], m_extent);
        }
    }
    void mesh_resource::unload()
    {
        m_submeshes.clear();
    }
    void mesh_resource::set_fbx_path(std::string path)
    {
        m_submeshes.clear();
        m_fbx_path = std::move(path);
        load();
    }

}

