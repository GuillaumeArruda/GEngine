#include "stdafx.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <optick/optick.h>

#include "grender/systems/render_system.h"

#include "gcore/components/transform_component.h"
#include "gcore/components/extent_component.h"
#include "gcore/world.h"
#include "gcore/console.h"


#include "grender/components/camera_component.h"
#include "grender/components/graphic_component.h"
#include "grender/components/light_component.h"
#include "grender/components/skybox_component.h"
#include "grender/resources/mesh_resource.h"
#include "grender/resources/program.h"
#include "grender/utils.h"
#include "grender/systems/debug_render_system.h"

namespace grender
{
    GCORE_REGISTER_CONSOLE_VALUE("render.enable", bool, enable_render, true);
    void render_system::update(gcore::world& world)
    {
        if (!enable_render)
            return;

        OPTICK_EVENT();

        gl_exec(glClear, GL_COLOR_BUFFER_BIT);
        gcore::entity_registry& registry = world.get_entity_registry();
        auto camera_view = registry.get_view<gcore::transform_component, camera_component>();       
        glm::ivec2 const size = get_target_size();
        float const aspect_ratio = size[0] / static_cast<float>(size[1]);
        gl_exec(glViewport, 0, 0, size[0], size[1]);
        for (auto& [entity, camera_transform, camera] : camera_view)
        {
            if (camera->m_active)
            {
                glm::mat4 const projection = glm::perspective(static_cast<float>(gmath::radian(camera->m_fov)), aspect_ratio, camera->m_near_z, camera->m_far_z);
                glm::mat4 const view_matrix = glm::inverse(camera_transform->m_transform);

                render_meshes(projection, view_matrix, registry);
                render_lights(camera_transform->m_transform, registry);
                render_skybox(projection, view_matrix, registry);

                if (debug_render_system* debug = world.get_system_registry().get_system<debug_render_system>())
                {
                    debug->draw_primitives(projection, view_matrix);
                }
            }
        }

        m_frame_buffer.unbind();
    }

    void render_system::connect_to_world(gcore::world& world)
    {
        auto graphic_comp_view = world.get_entity_registry().get_view<gcore::transform_component, graphic_component, gcore::optional_comp<gcore::extent_component>>();
        graphic_comp_view.add_on_added_callback([&](auto& added_entity) { this->on_mesh_entity_added(added_entity); });
        
        auto light_view = world.get_entity_registry().get_view<gcore::transform_component, light_component>();
        light_view.add_on_added_callback([&](auto& added_entity) { this->on_light_entity_added(added_entity); });
        
        auto skybox_view = world.get_entity_registry().get_view<grender::skybox_component>();
        skybox_view.add_on_added_callback([&](auto& added_entity) { this->on_skybox_entity_added(added_entity); });
    }

    void render_system::set_target_size(std::size_t width, std::size_t height)
    {
        if (m_frame_buffer.get_size() != glm::ivec2(width, height))
        {
            m_frame_buffer = frame_buffer(static_cast<GLsizei>(width), static_cast<GLsizei>(height));
        }
    }

    glm::ivec2 render_system::get_target_size() const
    {
        return m_frame_buffer.get_size();
    }

    GLuint render_system::get_target_id() const
    {
        return m_frame_buffer.get_render_target_id(frame_buffer::render_target_type::final_color);
    }

    void render_system::render_meshes(glm::mat4 const& projection, glm::mat4 const& view_matrix, gcore::entity_registry& registry)
    {
        OPTICK_EVENT();
        setup_geometry_pass();
        auto graphic_comp_view = registry.get_view<gcore::transform_component, graphic_component, gcore::optional_comp<gcore::extent_component>>();
        for (auto& [entity, transform, graphic_comp, extent] : graphic_comp_view)
        {
            glm::mat4 const mvp = projection * view_matrix * static_cast<glm::mat4>(transform->m_transform);
            glm::mat3 const normal_matrix = glm::transpose(glm::inverse(glm::mat3(transform->m_transform)));
            for (auto& mesh_info : graphic_comp->m_meshes)
            {
                if (mesh_info.m_active && mesh_info.m_mesh.is_loaded() && mesh_info.m_program.is_loaded())
                {
                    mesh_info.m_program->activate();
                    mesh_info.m_uniform_state.set_uniform("mvp", mvp);
                    mesh_info.m_uniform_state.set_uniform("normal_matrix", normal_matrix);
                    mesh_info.m_uniform_state.set_uniform("world_matrix", transform->m_transform);
                    mesh_info.m_uniform_state.apply();
                    mesh_info.m_mesh->draw();
                }
            }
        }
    }

    void render_system::render_lights(glm::mat4 const& camera_world_matrix, gcore::entity_registry& registry)
    {
        OPTICK_EVENT();
        setup_lightning_pass();
        gl_exec(glClear, GL_COLOR_BUFFER_BIT);
        auto light_view = registry.get_view<gcore::transform_component, light_component>();
        for (auto& [entity, transform, light] : light_view)
        {
            if (light->m_mesh.is_loaded())
            {
                if (light->m_main_program.is_loaded())
                {
                    light->m_main_state.reconcile(light->m_main_program->get_default_state());
                    setup_lightning_pass();
                    light->m_main_program->activate();
                    light->m_main_state.set_uniform("world_matrix", transform->m_transform);
                    light->m_main_state.set_uniform("screen_size", glm::vec2(get_target_size()));
                    light->m_main_state.set_uniform("camera_world_matrix", camera_world_matrix);
                    light->m_main_state.set_uniform("diffuse_tex", { m_frame_buffer.get_render_target_id(frame_buffer::render_target_type::diffuse), uniform_variant::type::sampler_2d });
                    light->m_main_state.set_uniform("position_tex", { m_frame_buffer.get_render_target_id(frame_buffer::render_target_type::position), uniform_variant::type::sampler_2d });
                    light->m_main_state.set_uniform("normal_tex", { m_frame_buffer.get_render_target_id(frame_buffer::render_target_type::normal), uniform_variant::type::sampler_2d });
                    light->m_main_state.set_uniform("specular_tex", { m_frame_buffer.get_render_target_id(frame_buffer::render_target_type::specular), uniform_variant::type::sampler_2d });
                    light->m_main_state.apply();
                    light->m_mesh->draw();
                }
            }
        }
        gl_exec(glDisable, GL_BLEND);
    }

    void render_system::render_skybox(glm::mat4 const& projection, glm::mat4 const& view_matrix, gcore::entity_registry& registry)
    {
        OPTICK_EVENT();
        setup_skybox_pass();
        auto view = registry.get_view<grender::skybox_component>();
        for (auto& [entity, skybox] : view)
        {
            if (skybox->m_program.is_loaded() && skybox->m_mesh.is_loaded())
            {
                skybox->m_program->activate();
                skybox->m_program_state.set_uniform("mvp", projection * glm::mat4(glm::mat3(view_matrix)));
                skybox->m_program_state.apply();
                skybox->m_mesh->draw();
            }
        }
    }

    void render_system::setup_geometry_pass()
    {
        m_frame_buffer.bind_for_geometry();
        gl_exec(glEnable, GL_DEPTH_TEST);
        gl_exec(glDepthMask, static_cast<GLboolean>(GL_TRUE));
        gl_exec(glDepthFunc, GL_LESS);
        gl_exec(glClear, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
    }

    void render_system::setup_lightning_pass()
    {
        m_frame_buffer.bind_for_light();
        //gl_exec(glStencilFunc, GL_NOTEQUAL, 0, 0xFF);
        gl_exec(glDisable, GL_DEPTH_TEST);
        gl_exec(glEnable, GL_BLEND);
        gl_exec(glBlendEquation, GL_FUNC_ADD);
        gl_exec(glBlendFunc, GL_ONE, GL_ONE);
        gl_exec(glEnable, GL_CULL_FACE);
        gl_exec(glCullFace, GL_BACK);
    }
    void render_system::setup_skybox_pass()
    {
        gl_exec(glEnable, GL_DEPTH_TEST);
        gl_exec(glDepthFunc, GL_LEQUAL);
        gl_exec(glDisable, GL_CULL_FACE);
        m_frame_buffer.bind_for_skybox();
    }

    void render_system::on_mesh_entity_added(std::tuple<gcore::entity, gcore::transform_component*, graphic_component*, gcore::extent_component*>& added_entity)
    {
        auto& [entity, transform, graphic_comp, extent] = added_entity;
        for (auto& mesh_info : graphic_comp->m_meshes)
        {
            if (mesh_info.m_mesh.is_loaded() && mesh_info.m_program.is_loaded())
            {
                mesh_info.m_uniform_state.reconcile(mesh_info.m_program->get_default_state());
                if (extent)
                {
                    extent->m_extent = extent->m_extent.merge(mesh_info.m_mesh->get_extent());
                }
            }
            else
            {
                mesh_info.m_active = false;
            }

        }
    }

    void render_system::on_light_entity_added(std::tuple<gcore::entity, gcore::transform_component*, light_component*>& added_entity)
    {
        auto& [entity, transform, light] = added_entity;
        if (light->m_mesh.is_loaded() && light->m_main_program.is_loaded())
        {
            light->m_main_state.reconcile(light->m_main_program->get_default_state());
        }
    }

    void render_system::on_skybox_entity_added(std::tuple<gcore::entity, skybox_component*>& added_entity)
    {
        auto& [entity, skybox] = added_entity;
        if (skybox->m_program.is_loaded())
        {
            skybox->m_program_state.reconcile(skybox->m_program->get_default_state());
        }
    }
}

