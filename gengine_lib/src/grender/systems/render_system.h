#pragma once

#include <glm/glm.hpp>

#include "gcore/system.h"

#include "grender/frame_buffer.h"
#include "gcore/entity_registry.h"

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
        void update(gcore::world& world);

        void set_target_size(std::size_t width, std::size_t height);
        glm::ivec2 get_target_size() const;
        GLuint get_target_id() const;

    private:
        void render_meshes(glm::mat4 const& projection, glm::mat4 const& view_matrix, gcore::entity_registry& registry);
        void render_lights(glm::mat4 const& camera_world_matrix, gcore::entity_registry& registry);
        void render_skybox(glm::mat4 const& projection, glm::mat4 const& view_matrix, gcore::entity_registry& registry);
        
        void setup_geometry_pass();
        void setup_lightning_pass();
        void setup_skybox_pass();
        
        void on_mesh_entity_added(std::tuple<gcore::entity, gcore::transform_component*, graphic_component*, gcore::extent_component*>& added_entity);
        void on_light_entity_added(std::tuple<gcore::entity, gcore::transform_component*, light_component*>& added_entity);
        void on_skybox_entity_added(std::tuple<gcore::entity, skybox_component*>& added_entity);

        frame_buffer m_frame_buffer = frame_buffer(0ull, 0ull);
    };
}