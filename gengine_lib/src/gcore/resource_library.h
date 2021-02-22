#pragma once

#include <unordered_map>
#include <string>
#include <memory>
#include <shared_mutex>
#include <filesystem>

#include "gtl/uuid.h"

#include "gthread/job_manager.h"

#include "gcore/resource.h"
#include "gcore/resource_handle.h"
#include "gcore/resource_dependency_tracker.h"

struct GLFWwindow;

namespace gserializer
{
    struct serializer;
}

namespace filewatch
{
    enum class event;
}

namespace gcore
{
    struct resource_library
    {
        resource_library(GLFWwindow* rendering_context);
        ~resource_library();

        void process(gserializer::serializer& serializer);

        template<class ResourceType>
        [[nodiscard]] resource_handle<ResourceType>  get_resource(gtl::uuid const& uuid)
        {
            return resource_handle<ResourceType>(get_resource(uuid));
        }

        template<class ResourceType>
        [[nodiscard]] resource_handle<ResourceType>  try_get_resource(gtl::uuid const& uuid)
        {
            return resource_handle<ResourceType>(try_get_resource(uuid));
        }

        std::filesystem::path get_filepath(gtl::uuid const& uuid) const;

        void scan_directory(const char* folder);
        void on_file_change(std::wstring const& path);

        void update();

        std::unordered_map<gtl::uuid, std::filesystem::path> const& get_uuid_to_resource_files() const { return m_uuid_to_resource_file; }

        void run_loading_job();

        resource_dependency_tracker& get_dependency_tracker() { return m_dependency_tracker; }
        resource_dependency_tracker const& get_dependency_tracker() const { return m_dependency_tracker; }

    private:
        friend struct resource_proxy;
        void request_unload(resource* res_to_unload);
        void load_resource(std::unique_ptr<resource> res_to_load);
        void reload_resource(gtl::uuid const& uuid);

        resource_handle<resource> get_resource(gtl::uuid const& uuid);
        resource_handle<resource> try_get_resource(gtl::uuid const& uuid);
        
        std::shared_mutex m_lock;
        std::unordered_map<gtl::uuid, std::filesystem::path> m_uuid_to_resource_file;
        std::unordered_map<gtl::uuid, std::unique_ptr<resource>> m_resource_map;
        std::unordered_map<gtl::uuid, std::weak_ptr<resource_proxy>> m_proxy_map;
        
        std::vector<std::unique_ptr<resource>> m_res_to_unload;
        std::vector<std::unique_ptr<resource>> m_res_to_load_sync;

        std::mutex m_file_change_lock;
        std::vector<std::filesystem::path> m_file_changes;

        resource_dependency_tracker m_dependency_tracker;

        gthread::job_manager m_jobs{ 1u };
        GLFWwindow* m_loading_context = nullptr;
    };
}