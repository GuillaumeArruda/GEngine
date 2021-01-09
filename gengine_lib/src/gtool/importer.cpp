#include "stdafx.h"

#include "gtool/importer.h"

#include <assimp/Importer.hpp>
#include <assimp/mesh.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#undef _UNICODE
#include <IL/il.h>
#define _UNICODE

#include <iostream>

#include "gserializer/gmath_serialization.h"
#include "gserializer/serializers/binary_file_serializer.h"
#include "gserializer/serializers/json_serializer.h"

#include "gcore/resource_library.h"

#include "grender/resources/mesh_resource.h"
#include "grender/resources/texture.h"

#include "gtl/scope_timer.h"



namespace gtool
{
    void import_model_file(std::string const& base_name, std::filesystem::path const& model_file_path, std::filesystem::path const& export_to_path, float scale_modifier)
    {
        Assimp::Importer importer;
        importer.SetPropertyInteger(AI_CONFIG_PP_SBP_REMOVE, aiPrimitiveType_POINT | aiPrimitiveType_LINE);
        aiScene const* scene = importer.ReadFile(model_file_path.string().c_str(),
            static_cast<unsigned int>(
                aiProcess_Triangulate |
                aiProcess_SortByPType |
                aiProcess_ImproveCacheLocality |
                aiProcess_FixInfacingNormals |
                aiProcess_OptimizeGraph |
                aiProcess_OptimizeMeshes |
                aiProcess_CalcTangentSpace |
                aiProcess_JoinIdenticalVertices |
                aiProcess_FindDegenerates |
                aiProcess_GenBoundingBoxes |
                aiProcess_FlipUVs));

        if (!scene)
        {
            std::cerr << importer.GetErrorString() << "\n";
            return;
        }

        std::string const new_base_name = base_name.size() == 0 ? model_file_path.stem().string() : base_name;
        details::import_meshes(new_base_name, export_to_path, *scene, scale_modifier);
        details::import_textures(new_base_name, model_file_path, export_to_path, *scene);
    }

    void import_texture_file(std::string const& base_name, std::filesystem::path const& texture_path, std::filesystem::path const& export_to_path)
    {
        ILuint const imageId = ilGenImage();
        ilBindImage(imageId);
        ILboolean const success = ilLoadImage(texture_path.string().c_str());
        auto error = ilGetError();
        if (!success)
        {
            ilDeleteImage(imageId);
            std::cerr << "Failed to import texture file: " << texture_path << " error: " << error<< "\n";
            return;
        }
        std::filesystem::path const folder_path = export_to_path.string() + "textures\\" + base_name + "\\";
        std::filesystem::path const export_path = folder_path.string() + texture_path.stem().string() + ".dds";

        bool const already_exist = std::filesystem::exists(export_path);
        std::filesystem::create_directories(folder_path);
        ILboolean const save_success = ilSaveImage(export_path.string().c_str());
        if (!save_success)
        {
            ilDeleteImage(imageId);
            std::cerr << "Failed to import texture file: " << texture_path << "\n";
            return;
        }

        ilDeleteImage(imageId);
        if (!already_exist)
        {
            auto texture_res = std::make_unique<grender::texture>();
            texture_res->set_name(base_name + "_" + export_path.stem().string());
            texture_res->set_filepath(export_path);

            std::unique_ptr<gcore::resource> res = std::move(texture_res);
            gserializer::json_write_serializer writer;
            writer.process("resource", res, gcore::resource::factory());
            writer.write_to_file((folder_path.string() + res->get_uuid().to_string() + ".json").c_str());
        }
    }
    
    void details::import_meshes(std::string const& base_name, std::filesystem::path const& export_to_path, aiScene const& scene, float scale_modifier)
    {
        for (unsigned int mesh_idx = 0; mesh_idx < scene.mNumMeshes; ++mesh_idx)
        {
            aiMesh const& mesh = *scene.mMeshes[mesh_idx];
            gtl::scope_timer s(mesh.mName.C_Str());
            grender::mesh_serialization_info info;

            info.m_min_extent.x = scale_modifier * mesh.mAABB.mMin.x;
            info.m_min_extent.y = scale_modifier * mesh.mAABB.mMin.y;
            info.m_min_extent.z = scale_modifier * mesh.mAABB.mMin.z;
            info.m_max_extent.x = scale_modifier * mesh.mAABB.mMax.x;
            info.m_max_extent.y = scale_modifier * mesh.mAABB.mMax.y;
            info.m_max_extent.z = scale_modifier * mesh.mAABB.mMax.z;
            
            if (mesh.HasPositions())
            {
                info.m_positions.reserve(mesh.mNumVertices);
                for (unsigned int i = 0; i < mesh.mNumVertices; ++i)
                {
                    info.m_positions.push_back(glm::vec3(mesh.mVertices[i].x, mesh.mVertices[i].y, mesh.mVertices[i].z) * scale_modifier);
                }
            }
            else
            {
                continue;
            }

            if (mesh.HasNormals())
            {
                info.m_normals.reserve(mesh.mNumVertices);
                for (unsigned int i = 0; i < mesh.mNumVertices; ++i)
                {
                    info.m_normals.push_back(glm::vec3(mesh.mNormals[i].x, mesh.mNormals[i].y, mesh.mNormals[i].z));
                }
            }

            if (mesh.HasTangentsAndBitangents())
            {
                info.m_tangents.reserve(mesh.mNumVertices);
                info.m_bitangents.reserve(mesh.mNumVertices);
                for (unsigned int i = 0; i < mesh.mNumVertices; ++i)
                {
                    info.m_tangents.push_back(glm::vec3(mesh.mTangents[i].x, mesh.mTangents[i].y, mesh.mTangents[i].z));
                    info.m_bitangents.push_back(glm::vec3(mesh.mBitangents[i].x, mesh.mBitangents[i].y, mesh.mBitangents[i].z));
                }
            }

            if (mesh.HasTextureCoords(0))
            {
                info.m_uv_coords.reserve(mesh.mNumVertices);
                for (unsigned int i = 0; i < mesh.mNumVertices; ++i)
                {
                    info.m_uv_coords.push_back(glm::vec2(mesh.mTextureCoords[0][i].x, mesh.mTextureCoords[0][i].y));
                }
            }

            if (mesh.HasFaces())
            {
                assert(mesh.mFaces[0].mNumIndices == 3);
                info.m_faces.reserve(mesh.mNumFaces);
                for (unsigned int i = 0; i < mesh.mNumFaces; ++i)
                {
                    info.m_faces.push_back(glm::ivec3(mesh.mFaces[i].mIndices[0], mesh.mFaces[i].mIndices[1], mesh.mFaces[i].mIndices[2]));
                }
            }

            std::string const mesh_name = base_name + "_" + mesh.mName.C_Str();
            std::filesystem::path const folder_path = export_to_path.string() + "meshes\\" + base_name + "\\";
            std::filesystem::path const mesh_file_path = folder_path.string() + mesh_name + ".meshbin";
            bool const already_exists = std::filesystem::exists(mesh_file_path);

            std::filesystem::create_directories(folder_path);
            gserializer::binary_file_writer bin_writer(mesh_file_path.string().c_str());
            bin_writer.process("mesh_info", info);

            if (!already_exists)
            {
                auto mesh_res = std::make_unique<grender::mesh_resource>();
                mesh_res->set_name(mesh_name);
                mesh_res->set_filepath(mesh_file_path);

                std::unique_ptr<gcore::resource> res = std::move(mesh_res);
                gserializer::json_write_serializer json_writer;
                json_writer.process("resource", res, gcore::resource::factory());
                json_writer.write_to_file((folder_path.string() + res->get_uuid().to_string() + ".json").c_str());
            }
        }
    }

    void details::import_textures(std::string const& base_name, std::filesystem::path const& model_file_path, std::filesystem::path const& export_to_path, aiScene const& scene)
    {
        std::vector<std::filesystem::path> paths;
        for (unsigned int mat_idx = 0; mat_idx < scene.mNumMaterials; ++mat_idx)
        {
            aiMaterial const& mat = *scene.mMaterials[mat_idx];
            for (unsigned int i = 0; i < aiTextureType_UNKNOWN; ++i)
            {
                for (unsigned int j = 0; j < mat.GetTextureCount(static_cast<aiTextureType>(i)); ++j)
                {
                    aiString file_path;
                    if (mat.GetTexture(static_cast<aiTextureType>(i), j, &file_path) == aiReturn_SUCCESS)
                    {
                        auto const it = std::find_if(paths.begin(), paths.end(),
                            [&](std::filesystem::path const& path) 
                            {
                                    return path == file_path.C_Str();
                            });
                        if (it == paths.end())
                        {
                            paths.emplace_back(file_path.C_Str());
                        }
                    }
                }
            }
        }

        for (auto const& file_path : paths)
        {
            import_texture_file(base_name, std::filesystem::absolute(model_file_path.parent_path() / file_path), export_to_path);
        }
    }
}


