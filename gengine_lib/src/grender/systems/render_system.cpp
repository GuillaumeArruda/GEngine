#include "stdafx.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <optick/optick.h>
#include <latch>

#include "grender/systems/render_system.h"

#include "gcore/components/transform_component.h"
#include "gcore/components/extent_component.h"
#include "gcore/world.h"
#include "gcore/console.h"

#include "gthread/job_manager.h"

#include "grender/components/camera_component.h"
#include "grender/components/graphic_component.h"
#include "grender/components/skybox_component.h"
#include "grender/resources/mesh_resource.h"
#include "grender/resources/program.h"
#include "grender/utils.h"
#include "grender/systems/debug_render_system.h"
#include "grender/nodes/render_mesh_node.h"

namespace grender
{
    GCORE_REGISTER_CONSOLE_VALUE("render.enable", bool, enable_render, true);
    void render_system::update(gcore::world& world, gthread::job_manager& jobs)
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

                run_graph_scripts(projection, view_matrix, registry, jobs);
                render_meshes();
                render_lights();
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
        m_graphic_comp_added_id = graphic_comp_view.add_on_added_callback([&](auto& added_entity) { this->on_mesh_entity_added(added_entity); });
        
        auto skybox_view = world.get_entity_registry().get_view<grender::skybox_component>();
        m_skybox_comp_added_id = skybox_view.add_on_added_callback([&](auto& added_entity) { this->on_skybox_entity_added(added_entity); });

        m_resource_library = world.get_resource_library();
    }

    void render_system::disconnect_from_world(gcore::world& world)
    {
        auto graphic_comp_view = world.get_entity_registry().get_view<gcore::transform_component, graphic_component, gcore::optional_comp<gcore::extent_component>>();
        graphic_comp_view.remove_on_added_callback(m_graphic_comp_added_id);

        auto skybox_view = world.get_entity_registry().get_view<grender::skybox_component>();
        skybox_view.remove_on_added_callback(m_skybox_comp_added_id);

        m_resource_library = nullptr;
    }

    void render_system::set_target_size(std::size_t width, std::size_t height)
    {
        if (m_frame_buffer.get_size() != glm::ivec2(width, height))
        {
            m_frame_buffer = frame_buffer(m_resource_library, static_cast<GLsizei>(width), static_cast<GLsizei>(height));
        }
    }

    glm::ivec2 render_system::get_target_size() const
    {
        return m_frame_buffer.get_size();
    }

    GLuint render_system::get_target_id() const
    {
        auto target = m_frame_buffer.get_render_target(frame_buffer::render_target_type::final_color);
        return target.is_loaded() ? target->get_id() : 0;
    }

    void render_system::add_mesh_draw_request(glm::mat4 const& transform, program_pin_data const& program, gcore::resource_handle<mesh_resource> mesh)
    {
        std::unique_lock lock(m_mesh_draw_requests_lock);
        m_mesh_draw_requests.emplace_back(transform, &program, std::move(mesh));
    }

    void render_system::add_light_draw_request(glm::mat4 const& transform, program_pin_data const& program, gcore::resource_handle<mesh_resource> mesh)
    {
        std::unique_lock lock(m_light_draw_requests_lock);
        m_light_draw_requests.emplace_back(transform, &program, std::move(mesh));
    }

    void render_system::run_graph_scripts(glm::mat4 const& projection, glm::mat4 const& view_matrix, gcore::entity_registry& registry, gthread::job_manager& jobs)
    {
        OPTICK_EVENT();
        auto graphic_comp_view = registry.get_view<gcore::transform_component, graphic_component, gcore::optional_comp<gcore::extent_component>>();
        std::latch latch(graphic_comp_view.size());
        for (auto& [entity, transform, graphic_comp, extent] : graphic_comp_view)
        {
            jobs.submit([=, &latch]() {
                if (graphic_comp->m_script.is_ready())
                {
                    OPTICK_EVENT("Graphic Script Update");
                    OPTICK_TAG("Script Name", graphic_comp->m_script.get_script()->get_name().c_str());

                    graphic_script_frame_input_data const input_data
                    {
                        transform->m_transform,
                        projection,
                        view_matrix
                    };

                    graphic_comp->m_script.set_in_context(input_data);
                    graphic_comp->m_script.execute();
                }
                latch.count_down();
            });
        }
        latch.wait();
    }

    void render_system::render_meshes()
    {
        OPTICK_EVENT();
        setup_geometry_pass();
        std::unique_lock lock(m_mesh_draw_requests_lock);
        for (auto& mesh_draw : m_mesh_draw_requests)
        {
            mesh_draw.program_data->m_program->activate();
            mesh_draw.program_data->m_state.apply();
            mesh_draw.m_mesh->draw();
        }
        m_mesh_draw_requests.clear();
    } 

    void render_system::render_lights()
    {
        OPTICK_EVENT();
        setup_lightning_pass();
        gl_exec(glClear, GL_COLOR_BUFFER_BIT);
        std::unique_lock lock(m_light_draw_requests_lock);
        for (auto& light_draw : m_light_draw_requests)
        {
            light_draw.program_data->m_program->activate();
            light_draw.program_data->m_state.apply();
            light_draw.m_mesh->draw();
        }
        m_light_draw_requests.clear();
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
        gl_exec(glStencilFunc, GL_NOTEQUAL, 0, 0xFF);
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

        if (graphic_comp->m_script.is_loaded())
        {
            for (gcore::node const* node : graphic_comp->m_script.get_script()->get_nodes())
            {
                if (grender::render_mesh_node const* render_mesh_node = dynamic_cast<grender::render_mesh_node const*>(node))
                {
                    if (gcore::resource_handle<grender::mesh_resource> const mesh = render_mesh_node->get_mesh();
                        mesh.is_loaded())
                    {
                        extent->m_extent = extent->m_extent.merge(mesh->get_extent());
                    }
                }
            }

            graphic_comp->m_script.set_in_context(std::reference_wrapper(*this));
            graphic_comp->m_script.prepare(m_resource_library);

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

