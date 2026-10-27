#pragma once

#include <glm/glm.hpp>

#include "gcore/system.h"

#include "gcore/entity_registry.h"
#include "grender/frame_buffer.h"
#include "grender/resources/mesh_resource.h"
#include "gthread/locked.h"

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
    struct program_pin_data;

    struct render_system : gcore::system
    {
        struct draw_request
        {
            glm::mat4 m_transform;
            program_pin_data const* m_program_data = nullptr;
            gcore::resource_handle<mesh_resource> m_mesh;
        };

        struct mesh_draw_request : draw_request
        {
            program_pin_data const* m_shadow_program = nullptr;
        };

        struct light_draw_request : draw_request
        {
            bool m_cull_back = false;
        };

        void connect_to_world(gcore::world& world) override;
        void disconnect_from_world(gcore::world& world) override;
        void update(gcore::world& world, gthread::job_manager& jobs);

        void set_target_size(std::size_t width, std::size_t height);
        glm::ivec2 get_target_size() const;
        GLuint get_target_id() const;

        void add_mesh_draw_request(glm::mat4 const& transform, program_pin_data const& program, program_pin_data const& shadow_program, gcore::resource_handle<mesh_resource> mesh);
        void add_light_draw_request(glm::mat4 const& transform, program_pin_data const& program, gcore::resource_handle<mesh_resource> mesh, bool cull_back_face);
        void add_skybox_draw_request(glm::mat4 const& transform, program_pin_data const& program, gcore::resource_handle<mesh_resource> mesh);
    
        template<class Functor>
        void add_pre_light_work(Functor&& functor)
        {
            m_pre_light_works.get()->emplace_back(std::forward<Functor>(functor));
        }

        template<class Functor>
        void for_each_mesh_draw_request(Functor&& functor)
        {
            auto locked_draw_request = m_mesh_draw_requests.get();
            for (mesh_draw_request const& request : locked_draw_request.m_data)
            {
                functor(request);
            }
        }

        gcore::resource_handle<texture> get_render_target(frame_buffer::render_target_type render_type) const { return m_frame_buffer.get_render_target(render_type); }
    
    private:
        void run_graph_scripts(glm::mat4 const& projection, glm::mat4 const& view_matrix, gcore::entity_registry& registry, gthread::job_manager& jobs);
        void render_meshes();
        void render_lights();
        void render_skybox();

        void setup_geometry_pass();
        void setup_lightning_pass();
        void setup_skybox_pass();
        
        void on_graph_comp_added(std::tuple<gcore::entity, graphic_component*, gcore::transform_component*, gcore::extent_component*>& added_entity);

        gcore::resource_library* m_resource_library = nullptr;
        frame_buffer m_frame_buffer = frame_buffer(nullptr, 0ull, 0ull);
        gthread::locked<std::vector<mesh_draw_request>> m_mesh_draw_requests;
        gthread::locked<std::vector<std::move_only_function<void(render_system&)>>> m_pre_light_works;
        gthread::locked<std::vector<light_draw_request>> m_light_draw_requests;
        gthread::locked<std::vector<draw_request>> m_skybox_draw_requests;
        gtl::callback_id m_graphic_comp_added_id = 0;
    };
} 