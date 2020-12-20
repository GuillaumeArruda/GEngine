#include "stdafx.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "grender/systems/render_system.h"

#include "gcore/components/transform_component.h"
#include "gcore/components/extent_component.h"
#include "gcore/entity_registry.h"
#include "gcore/resource_library.h"


#include "grender/components/camera_component.h"
#include "grender/components/graphic_component.h"
#include "grender/components/light_component.h"
#include "grender/resources/mesh_resource.h"
#include "grender/resources/program.h"
#include "grender/utils.h"

namespace grender
{
    void render_system::render(gcore::entity_registry& registry, gcore::resource_library& library)
    {
        gl_exec(glClear, GL_COLOR_BUFFER_BIT);
        auto camera_view = registry.get_view<gcore::transform_component, camera_component>();       
        for (auto& [entity, camera_transform, camera] : camera_view)
        {
            if (camera->m_active)
            {
                glm::mat4 const projection = glm::perspective(static_cast<float>(gmath::radian(camera->m_fov)), 16.f / 9.f, camera->m_near_z, camera->m_far_z);
                glm::mat4 const view_matrix = camera_transform->m_transform;

                render_meshes(projection, view_matrix, registry, library);
                render_lights(projection, view_matrix, registry, library);
            }
            m_frame_buffer.transfer_to_default();
            m_frame_buffer.unbind();
        }
       
    }

    void render_system::render_meshes(glm::mat4 const& projection, glm::mat4 const& view_matrix, gcore::entity_registry& registry, gcore::resource_library& library)
    {
        setup_geometry_pass();
        auto graphic_comp_view = registry.get_view<gcore::transform_component, graphic_component>();
        for (auto& [entity, transform, graphic_comp] : graphic_comp_view)
        {
            glm::mat4 const mvp = projection * view_matrix * static_cast<glm::mat4>(transform->m_transform);
            glm::mat3 const normal_matrix = glm::transpose(glm::inverse(glm::mat3(transform->m_transform)));
            for (auto& mesh_info : graphic_comp->m_meshes)
            {
                if (!mesh_info.m_mesh || !mesh_info.m_program
                    || mesh_info.m_mesh->get_uuid() != mesh_info.m_mesh_id
                    || mesh_info.m_program->get_uuid() != mesh_info.m_program_id)
                {
                    if (mesh_resource const* mesh = library.get_resource<mesh_resource>(mesh_info.m_mesh_id))
                    {
                        if (program const* program_res = library.get_resource<program>(mesh_info.m_program_id))
                        {
                            mesh_info.m_mesh = mesh;
                            mesh_info.m_program = program_res;
                            mesh_info.m_uniform_state.reconcile(program_res->get_default_state());
                            if (auto extent_comp = registry.get_components<gcore::extent_component>(entity))
                            {
                                extent_comp->m_extent = extent_comp->m_extent.merge(mesh->get_extent());
                            }
                        }
                    }
                }

                if (mesh_info.m_mesh && mesh_info.m_program)
                {
                    auto const world_matrix_id = mesh_info.m_uniform_state.get_uniform_location("world_matrix");
                    mesh_info.m_program->activate();
                    mesh_info.m_uniform_state.set_uniform("mvp", mvp);
                    mesh_info.m_uniform_state.set_uniform("normal_matrix", normal_matrix);
                    mesh_info.m_uniform_state.set_uniform("world_matrix", transform->m_transform);
                    mesh_info.m_uniform_state.apply(library);
                    for (auto& submesh : mesh_info.m_mesh->get_meshes())
                    {
                        submesh.draw();
                    }
                }
            }
        }
    }

    void render_system::render_lights(glm::mat4 const& projection, glm::mat4 const& view_matrix, gcore::entity_registry& registry, gcore::resource_library& library)
    {
        m_frame_buffer.bind_for_light();
        gl_exec(glClear, GL_COLOR_BUFFER_BIT);
        gl_exec(glEnable, GL_STENCIL_TEST);
        auto light_view = registry.get_view<gcore::transform_component, light_component>();
        for (auto& [entity, transform, light] : light_view)
        {
            if (mesh_resource const* mesh = library.get_resource<mesh_resource>(light->m_mesh))
            {
                if (program const* stencil_program = library.get_resource<program>(light->m_stencil_program))
                {
                    light->m_stencil_state.reconcile(stencil_program->get_default_state());
                    setup_stencil_pass();
                    stencil_program->activate();
                    auto const world_matrix_loc = light->m_main_state.get_uniform_location("world_matrix");
                    auto const screen_size = light->m_main_state.get_uniform_location("screen_size");
                    light->m_main_state.set_uniform("world_matrix", transform->m_transform);
                    light->m_main_state.set_uniform("screen_size", glm::vec2(1920.f, 1080.f));
                    light->m_stencil_state.apply(library);
                    for (auto& submesh : mesh->get_meshes())
                    {
                        submesh.draw();
                    }
                }

                if (program const* light_program = library.get_resource<program>(light->m_main_program))
                {
                    light->m_main_state.reconcile(light_program->get_default_state());
                    setup_lightning_pass();
                    light_program->activate();
                    light->m_main_state.set_uniform("world_matrix", transform->m_transform);
                    light->m_main_state.set_uniform("scren_size", glm::vec2(1920.f, 1080.f));
                    light->m_main_state.set_uniform("camera_world_matrix", glm::inverse(view_matrix));
                    light->m_main_state.set_uniform("diffuse_tex", { m_frame_buffer.get_render_target_id(frame_buffer::render_target_type::diffuse), uniform_variant::type::sampler_2d });
                    light->m_main_state.set_uniform("position_tex", { m_frame_buffer.get_render_target_id(frame_buffer::render_target_type::position), uniform_variant::type::sampler_2d });
                    light->m_main_state.set_uniform("normal_tex", { m_frame_buffer.get_render_target_id(frame_buffer::render_target_type::normal), uniform_variant::type::sampler_2d });
                    light->m_main_state.set_uniform("specular_tex", { m_frame_buffer.get_render_target_id(frame_buffer::render_target_type::specular), uniform_variant::type::sampler_2d });
                    light->m_main_state.apply(library);
                    for (auto& submesh : mesh->get_meshes())
                    {
                        submesh.draw();
                    }
                }
            }
        }
        gl_exec(glDisable, GL_STENCIL_TEST);
        gl_exec(glDisable, GL_BLEND);
    }

    void render_system::setup_geometry_pass()
    {
        m_frame_buffer.bind_for_geometry();
        gl_exec(glEnable, GL_DEPTH_TEST);
        gl_exec(glDepthMask, GL_TRUE);
        gl_exec(glClear, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void render_system::setup_stencil_pass()
    {
        m_frame_buffer.bind_for_stencil();
        gl_exec(glEnable, GL_DEPTH_TEST);
        gl_exec(glDepthMask, GL_FALSE);
        gl_exec(glDisable, GL_CULL_FACE);
        gl_exec(glClear, GL_STENCIL_BUFFER_BIT);
        gl_exec(glStencilFunc, GL_ALWAYS, 0, 0);
        gl_exec(glStencilOpSeparate, GL_BACK, GL_KEEP, GL_DECR_WRAP, GL_KEEP);
        gl_exec(glStencilOpSeparate, GL_FRONT, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
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
}

