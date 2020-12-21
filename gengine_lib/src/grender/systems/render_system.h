#pragma once

#include <glm/glm.hpp>

#include "gcore/system.h"

#include "grender/frame_buffer.h"

namespace gcore
{
    struct entity_registry;
}

namespace grender
{
    struct render_system : gcore::system
    {
        void render(gcore::entity_registry& registry);

        void set_target_size(std::size_t width, std::size_t height);
        glm::ivec2 get_target_size() const;
        GLuint get_target_id() const;

    private:
        void render_meshes(glm::mat4 const& projection, glm::mat4 const& view_matrix, gcore::entity_registry& registry);
        void render_lights(glm::mat4 const& projection, glm::mat4 const& view_matrix, gcore::entity_registry& registry);
        void render_skybox(glm::mat4 const& projection, glm::mat4 const& view_matrix, gcore::entity_registry& registry);
        
        void setup_geometry_pass();
        void setup_stencil_pass();
        void setup_lightning_pass();
        void setup_skybox_pass();

        frame_buffer m_frame_buffer = frame_buffer(0ull, 0ull);
    };
}