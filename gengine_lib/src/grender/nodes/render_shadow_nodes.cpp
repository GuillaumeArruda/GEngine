#include "stdafx.h"

#include <gl/gl.h>
#include <glm/gtc/matrix_transform.hpp>
#include <optick/optick.h>

#include "gcore/resource_library.h"
#include "gmath/units.h"

#include "grender/nodes/render_shadow_nodes.h"
#include "grender/resources/runtime_texture.h"
#include "grender/systems/render_system.h"
#include "grender/utils.h"


namespace grender
{
    void render_directional_shadow_node::prepare(gcore::node_context& context) const
    {
        if (auto* res_lib = context.get_in_context<std::reference_wrapper<gcore::resource_library>>())
        {
            auto texture = std::make_unique<runtime_texture>();
            auto fbo = std::make_unique<frame_buffer_resource>();
            auto tex_handle = res_lib->get().add_runtime_resource(std::move(texture));
            auto fbo_handle = res_lib->get().add_runtime_resource(std::move(fbo));
            context.m_node_memory = state{tex_handle, fbo_handle};
        }
    }

    void render_directional_shadow_node::execute(gcore::node_context& context) const
    {
        constexpr int shadow_map_size = 8192;
        if (auto* render_sys = context.get_in_context<std::reference_wrapper<render_system>>())
        {
            auto& memory = std::any_cast<state&>(context.m_node_memory);
            if (memory.m_texture->get_id() == 0 && memory.m_framebuffer->get_fbo() == 0)
            {
                render_sys->get().add_pre_light_work( 
                    [memory = memory](render_system&) mutable
                    {
                        OPTICK_EVENT("Create directional shadow resources");
                        GLuint texture_id = 0;
                        gl_exec(glGenTextures, 1, &texture_id);
                        gl_exec(glBindTexture, GL_TEXTURE_2D, texture_id);
                        gl_exec(glTexImage2D, GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadow_map_size, shadow_map_size, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
                        gl_exec(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                        gl_exec(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                        gl_exec(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
                        gl_exec(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
                        constexpr float bordercolor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
                        gl_exec(glTexParameterfv, GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, bordercolor);
                        memory.m_texture->set_id(texture_id);

                        GLuint fbo_id = 0;
                        gl_exec(glGenFramebuffers, 1, &fbo_id);
                        gl_exec(glBindFramebuffer, GL_FRAMEBUFFER, fbo_id);
                        gl_exec(glFramebufferTexture2D, GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture_id, 0);
                        gl_exec(glDrawBuffer, GL_NONE);
                        gl_exec(glReadBuffer, GL_NONE);
                        gl_exec(glBindFramebuffer, GL_FRAMEBUFFER, 0);
                        memory.m_framebuffer->set_fbo(fbo_id);
                    }
                );
            }

            auto programs = context.read<in_program_pin>();
            auto camera_matrixs = context.read<in_camera_matrix_pin>();
            auto camera_projections = context.read<in_camera_matrix_projection_pin>();
            auto light_directions = context.read<in_light_direction_pin>();
            if (programs.empty() || camera_matrixs.empty() || camera_projections.empty() || light_directions.empty())
                return;

            std::array<glm::vec4, 8> const corners = get_camera_frustrum_corners(camera_projections[0], camera_matrixs[0]);
            glm::mat4 const light_matrix = create_light_matrix(light_directions[0], corners);

            render_sys->get().add_pre_light_work(
                [memory = memory, program = programs[0], light_matrix](render_system& render_sys) mutable
                {
                    OPTICK_EVENT("Render directional shadow");
                    gl_exec(glBindFramebuffer, GL_FRAMEBUFFER, memory.m_framebuffer->get_fbo());
                    gl_exec(glViewport, 0, 0, shadow_map_size, shadow_map_size);
                    gl_exec(glClear, GL_DEPTH_BUFFER_BIT);
                    gl_exec(glCullFace, GL_FRONT);
                    program.m_program->activate();
                    render_sys.for_each_mesh_draw_request(
                        [&program, &light_matrix](render_system::draw_request const& request) mutable
                        {
                            glm::mat4 const mvp = light_matrix * request.m_transform;
                            program.m_state.set_uniform("mvp", mvp);
                            program.m_state.apply();
                            request.m_mesh->draw();
                        }
                    );
                    gl_exec(glCullFace, GL_BACK);
                    gl_exec(glBindFramebuffer, GL_FRAMEBUFFER, 0);
                }
            );
            
            context.write_values<out_texture_pin>(gcore::resource_handle<texture>(memory.m_texture));
            context.write_values<out_light_matrix_pin>(light_matrix);
        }
    }

    gcore::node::pin_descriptors render_directional_shadow_node::get_pin_descriptors() const
    {
        static gcore::pin_descriptor const inputs[] = {
            in_program_pin::get("program"),
            in_light_direction_pin::get("light direction"),
            in_camera_matrix_pin::get("camera matrix"),
            in_camera_matrix_projection_pin::get("camera projection"),
        };

        static gcore::pin_descriptor const outputs[] = {
            out_texture_pin::get("shadow texture"),
            out_light_matrix_pin::get("light matrix"),
        };
        return node::pin_descriptors{inputs, outputs};
    }

    std::array<glm::vec4, 8> render_directional_shadow_node::get_camera_frustrum_corners(glm::mat4 const& projection, glm::mat4 const& view)
    {
        std::array<glm::vec4, 8> return_value;
        glm::mat4 const inv = glm::inverse(projection * view);
        for (int x = 0; x < 2; ++x)
            for (int y = 0; y < 2; ++y)
                for (int z = 0; z < 2; ++z)
                {
                    glm::vec4 const corner = inv * glm::vec4(2.f * x - 1.f, 2.f * y - 1.f, 2.f * z - 1.f, 1.f);
                    return_value[x * 4 + y * 2 + z] = corner / corner.w;
                }

        return return_value;
    }

    glm::mat4 render_directional_shadow_node::create_light_matrix(glm::vec3 light_direction, std::array<glm::vec4, 8> const& corners)
    {
        glm::vec3 center = glm::vec3(0.f);
        for (auto const& corner : corners)
        {
            center += glm::vec3(corner);
        }
        center /= corners.size();

        float radius = 0.f;
        for (auto const& corner : corners)
        {
            radius = std::max(glm::distance(glm::vec3(corner), center), radius);
        }

        glm::vec3 const max_extent = glm::vec3(radius);
        glm::vec3 const min_extent = glm::vec3(-radius);

        glm::mat4 const light_view_matrix = glm::lookAt(center - glm::normalize(light_direction) * min_extent.z, center, generate_valid_up(light_direction));
        glm::mat4 const light_projection_matrix = glm::ortho(min_extent.x, max_extent.x, min_extent.y, max_extent.y, 0.f, max_extent.z - min_extent.z);
        return light_projection_matrix * light_view_matrix;
    }

    void render_point_light_shadow_node::prepare(gcore::node_context& context) const
    {
        if (auto* res_lib = context.get_in_context<std::reference_wrapper<gcore::resource_library>>())
        {
            auto texture = std::make_unique<runtime_texture>();
            auto fbo = std::make_unique<frame_buffer_resource>();
            auto tex_handle = res_lib->get().add_runtime_resource(std::move(texture));
            auto fbo_handle = res_lib->get().add_runtime_resource(std::move(fbo));
            context.m_node_memory = state{ tex_handle, fbo_handle };
        }
    }
    
    void render_point_light_shadow_node::execute(gcore::node_context& context) const
    {
        constexpr int shadow_map_size = 1024;
        if (auto* render_sys = context.get_in_context<std::reference_wrapper<render_system>>())
        {
            auto& memory = std::any_cast<state&>(context.m_node_memory);
            if (memory.m_texture->get_id() == 0 && memory.m_framebuffer->get_fbo() == 0)
            {
                render_sys->get().add_pre_light_work(
                    [memory = memory](render_system&) mutable
                {
                    OPTICK_EVENT("Create point light shadow resources");
                    GLuint texture_id = 0;
                    gl_exec(glGenTextures, 1, &texture_id);
                    gl_exec(glBindTexture, GL_TEXTURE_CUBE_MAP, texture_id);
                    for (int i = 0; i < 6; ++i)
                    {
                        gl_exec(glTexImage2D, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, shadow_map_size, shadow_map_size, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
                    }

                    gl_exec(glTexParameteri, GL_TEXTURE_CUBE_MAP, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
                    gl_exec(glTexParameteri, GL_TEXTURE_CUBE_MAP, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
                    gl_exec(glTexParameteri, GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                    gl_exec(glTexParameteri, GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                    gl_exec(glTexParameteri, GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                    gl_exec(glTexParameteri, GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                    gl_exec(glTexParameteri, GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
                    memory.m_texture->set_id(texture_id);

                    GLuint fbo_id = 0;
                    gl_exec(glGenFramebuffers, 1, &fbo_id);
                    gl_exec(glBindFramebuffer, GL_FRAMEBUFFER, fbo_id);
                    gl_exec(glFramebufferTexture, GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, texture_id, 0);
                    gl_exec(glDrawBuffer, GL_NONE);
                    gl_exec(glReadBuffer, GL_NONE);
                    gl_exec(glBindFramebuffer, GL_FRAMEBUFFER, 0);
                    memory.m_framebuffer->set_fbo(fbo_id);
                }
                );
            }

            auto programs = context.read<in_program_pin>();
            auto matrixes = context.read<in_light_matrix_pin>();
            auto radii = context.read<in_radius_pin>();
            if (programs.empty() || matrixes.empty() || radii.empty())
                return;

            render_sys->get().add_pre_light_work(
                [memory, program = programs[0], light_pos = glm::vec3(matrixes[0][3]), radius = radii[0]](render_system& render_sys) mutable
            {
                OPTICK_EVENT("Render point light shadow");
                gl_exec(glBindFramebuffer, GL_FRAMEBUFFER, memory.m_framebuffer->get_fbo());
                gl_exec(glViewport, 0, 0, shadow_map_size, shadow_map_size);
                gl_exec(glClear, GL_DEPTH_BUFFER_BIT);
                gl_exec(glCullFace, GL_FRONT);
                program.m_program->activate();

                constexpr std::array directions = {
                    glm::vec3(1.f, 0.f, 0.f),
                    glm::vec3(-1.f, 0.f, 0.f),
                    glm::vec3(0.f, 1.f, 0.f),
                    glm::vec3(0.f,-1.f, 0.f),
                    glm::vec3(0.f, 0.f, 1.f),
                    glm::vec3(0.f, 0.f, -1.f)
                };

                constexpr std::array up_vectors = {
                    glm::vec3(0.f, -1.f, 0.f),
                    glm::vec3(0.f, -1.f, 0.f),
                    glm::vec3(0.f, 0.f, 1.f),
                    glm::vec3(0.f, 0.f, -1.f),
                    glm::vec3(0.f, -1.f, 0.f),
                    glm::vec3(0.f, -1.f, 0.f)
                };

                glm::mat4 const light_projection = glm::perspective(gmath::radian(90.0_d).value(), 1.f, 0.1f, radius);
                for (int i = 0; i < 6; ++i)
                {
                    glm::mat4 const light_view_matrix = glm::lookAt(light_pos, light_pos + directions[i], up_vectors[i]);
                    glm::mat4 const light_matrix = light_projection * light_view_matrix;
                    gl_exec(glFramebufferTexture2D, GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, memory.m_texture->get_id(), 0);
                    gl_exec(glClear, GL_DEPTH_BUFFER_BIT);
                    render_sys.for_each_mesh_draw_request(
                        [&program, &light_matrix](render_system::draw_request const& request) mutable
                        {
                            glm::mat4 const mvp = light_matrix * request.m_transform;
                            program.m_state.set_uniform("mvp", mvp);
                            program.m_state.apply();
                            request.m_mesh->draw();
                        }
                    );
                }

                gl_exec(glCullFace, GL_BACK);
                gl_exec(glBindFramebuffer, GL_FRAMEBUFFER, 0);
            }
            );

            context.write_values<out_texture_pin>(gcore::resource_handle<texture>(memory.m_texture));
        }
    }

    gcore::node::pin_descriptors render_point_light_shadow_node::get_pin_descriptors() const
    {
        static gcore::pin_descriptor const inputs[] = {
            in_program_pin::get("program"),
            in_light_matrix_pin::get("light matrix"),
            in_radius_pin::get("radius"),
        };

        static gcore::pin_descriptor const outputs[] = {
            out_texture_pin::get("shadow texture"),
        };
        return node::pin_descriptors{ inputs, outputs };
    }
}
