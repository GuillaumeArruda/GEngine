#pragma once

struct aiScene;

namespace gtool
{
    void import_model_file(std::string const& base_name, std::filesystem::path const& model_file_path, std::filesystem::path const& export_to_path);
    void import_texture_file(std::string const& base_name, std::filesystem::path const& texture_path, std::filesystem::path const& export_to_path);

    namespace details
    {
        void import_meshes(std::string const& base_name, std::filesystem::path const& export_to_path, aiScene const& scene);
        void import_textures(std::string const& base_name, std::filesystem::path const& model_file_path, std::filesystem::path const& export_to_path, aiScene const& scene);
    }

}