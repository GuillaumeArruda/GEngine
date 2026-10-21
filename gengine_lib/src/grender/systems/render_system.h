#pragma once

#include <glm/glm.hpp>

#include "gcore/system.h"

#include "grender/frame_buffer.h"
#include "gcore/entity_registry.h"
#include "grender/resources/mesh_resource.h"

namespace gthread
{
    struct job_manager;
}

namespace gcore
{
    struct world;
    struct entity_registry;
    struct transform_component;
    struct extent_component;
}

namespace grender
{
    struct graphic_component;
    struct light_component;
    struct skybox_component;

    struct render_system : gcore::system
    {
        void connect_to_world(gcore::world& world) override;
        void disconnect_from_world(gcore::world& world) override;
        void update(gcore::world& world, gthread::job_manager& jobs);

        void set_target_size(std::size_t width, std::size_t height);
        glm::ivec2 get_target_size() const;
        GLuint get_target_id() const;

        void add_mesh_draw_request(glm::mat4 const& transform, struct program_pin_data const& program, gcore::resource_handle<mesh_resource> mesh);
    private:
        struct mesh_draw_request
        {
            glm::mat4 m_transform;
            struct program_pin_data const* program_data = nullptr;
            gcore::resource_handle<mesh_resource> m_mesh;
        };

        void render_meshes(glm::mat4 const& projection, glm::mat4 const& view_matrix, gcore::entity_registry& registry, gthread::job_manager& jobs);
        void render_lights(glm::mat4 const& camera_world_matrix, gcore::entity_registry& registry);
        void render_skybox(glm::mat4 const& projection, glm::mat4 const& view_matrix, gcore::entity_registry& registry);
        
        void setup_geometry_pass();
        void setup_lightning_pass();
        void setup_skybox_pass();
        
        void on_mesh_entity_added(std::tuple<gcore::entity, gcore::transform_component*, graphic_component*, gcore::extent_component*>& added_entity);
        void on_light_entity_added(std::tuple<gcore::entity, gcore::transform_component*, light_component*>& added_entity);
        void on_skybox_entity_added(std::tuple<gcore::entity, skybox_component*>& added_entity);

        gcore::resource_library* m_resource_library = nullptr;
        frame_buffer m_frame_buffer = frame_buffer(0ull, 0ull);
        std::mutex m_mesh_draw_request_lock;
        std::vector<mesh_draw_request> m_mesh_draw_requests;
        gtl::callback_id m_graphic_comp_added_id = 0;
        gtl::callback_id m_light_comp_added_id = 0;
        gtl::callback_id m_skybox_comp_added_id = 0;
    };
}