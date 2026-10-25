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
                render_skybox();

                if (debug_render_system* debug = world.get_system_registry().get_system<debug_render_system>())
                {
                    debug->draw_primitives(projection, view_matrix);
                }
            }
            m_mesh_draw_requests.get()->clear();
            m_pre_light_works.get()->clear();
            m_light_draw_requests.get()->clear();
            m_skybox_draw_requests.get()->clear();
        }

        m_frame_buffer.unbind();
    }

    void render_system::connect_to_world(gcore::world& world)
    {
        auto graphic_comp_view = world.get_entity_registry().get_view<graphic_component, gcore::optional_comp<gcore::transform_component>, gcore::optional_comp<gcore::extent_component>>();
        m_graphic_comp_added_id = graphic_comp_view.add_on_added_callback([&](auto& added_entity) { this->on_graph_comp_added(added_entity); });
        
        m_resource_library = world.get_resource_library();
    }

    void render_system::disconnect_from_world(gcore::world& world)
    {
        auto graphic_comp_view = world.get_entity_registry().get_view<graphic_component, gcore::optional_comp<gcore::transform_component>, gcore::optional_comp<gcore::extent_component>>();
        graphic_comp_view.remove_on_added_callback(m_graphic_comp_added_id);

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
        m_mesh_draw_requests.get()->emplace_back(transform, &program, std::move(mesh));
    }

    void render_system::add_light_draw_request(glm::mat4 const& transform, program_pin_data const& program, gcore::resource_handle<mesh_resource> mesh)
    {
        m_light_draw_requests.get()->emplace_back(transform, &program, std::move(mesh));
    }

    void render_system::add_skybox_draw_request(glm::mat4 const& transform, program_pin_data const& program, gcore::resource_handle<mesh_resource> mesh)
    {
        m_skybox_draw_requests.get()->emplace_back(transform, &program, std::move(mesh));
    }

    void render_system::run_graph_scripts(glm::mat4 const& projection, glm::mat4 const& view_matrix, gcore::entity_registry& registry, gthread::job_manager& jobs)
    {
        OPTICK_EVENT();
        auto graphic_comp_view = registry.get_view<graphic_component, gcore::optional_comp<gcore::transform_component>, gcore::optional_comp<gcore::extent_component>>();
        std::latch latch(graphic_comp_view.size());
        for (auto& [entity, graphic_comp, transform, extent] : graphic_comp_view)
        {
            jobs.submit([=, &latch]() {
                if (graphic_comp->m_script.is_ready())
                {
                    OPTICK_EVENT("Graphic Script Update");
                    OPTICK_TAG("Script Name", graphic_comp->m_script.get_script()->get_name().c_str());

                    graphic_script_frame_input_data const input_data
                    {
                        transform ? transform->m_transform : glm::mat4(1.0f),
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
        execute_draw_requests(m_mesh_draw_requests);
    } 

    void render_system::render_lights()
    {
        OPTICK_EVENT();
        {
            OPTICK_EVENT("Pre Light Work")
            auto locked_pre_light_work = m_pre_light_works.get();
            for (auto& work : locked_pre_light_work.m_data)
            {
                work(*this);
            }
        }

        setup_lightning_pass();
        execute_draw_requests(m_light_draw_requests);
    }

    void render_system::render_skybox()
    {
        OPTICK_EVENT();
        setup_skybox_pass();
        execute_draw_requests(m_skybox_draw_requests);
    }

    void render_system::execute_draw_requests(gthread::locked<std::vector<draw_request>>& draw_requests)
    {
        auto locked_draw_requests = draw_requests.get();
        for (auto& draw_request : locked_draw_requests.m_data)
        {
            draw_request.program_data->m_program->activate();
            draw_request.program_data->m_state.apply();
            draw_request.m_mesh->draw();
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
        glm::ivec2 const size = get_target_size();
        gl_exec(glViewport, 0, 0, size[0], size[1]);
        gl_exec(glStencilFunc, GL_NOTEQUAL, 0, 0xFF);
        gl_exec(glDisable, GL_DEPTH_TEST);
        gl_exec(glEnable, GL_BLEND);
        gl_exec(glBlendEquation, GL_FUNC_ADD);
        gl_exec(glBlendFunc, GL_ONE, GL_ONE);
        gl_exec(glEnable, GL_CULL_FACE);
        gl_exec(glCullFace, GL_BACK);
        gl_exec(glClear, GL_COLOR_BUFFER_BIT);
    }
    void render_system::setup_skybox_pass()
    {
        gl_exec(glDisable, GL_BLEND);
        gl_exec(glEnable, GL_DEPTH_TEST);
        gl_exec(glDepthFunc, GL_LEQUAL);
        gl_exec(glDisable, GL_CULL_FACE);
        m_frame_buffer.bind_for_skybox();
    }

    void render_system::on_graph_comp_added(std::tuple<gcore::entity,graphic_component*, gcore::transform_component*, gcore::extent_component*>& added_entity)
    {
        auto& [entity, graphic_comp, transform, extent] = added_entity;

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

            graphic_comp->m_script.set_in_context(std::reference_wrapper(*m_resource_library));
            graphic_comp->m_script.set_in_context(std::reference_wrapper(*this));
            graphic_comp->m_script.prepare(m_resource_library);

        }
    }
}

