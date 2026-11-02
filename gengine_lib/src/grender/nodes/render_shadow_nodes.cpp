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
    void render_shadow_node_base::prepare(gcore::node_context& context) const
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

    void render_shadow_node_base::process(gserializer::serializer& serializer)
    {
        node::process(serializer);
        serializer.process("texture size", m_texture_size);
    }

    void render_directional_shadow_node::execute(gcore::node_context& context) const
    {
        if (auto* render_sys = context.get_in_context<std::reference_wrapper<render_system>>())
        {
            auto& memory = std::any_cast<state&>(context.m_node_memory);
            if (memory.m_texture->get_id() == 0 && memory.m_framebuffer->get_fbo() == 0)
            {
                render_sys->get().add_pre_light_work( 
                    [memory = memory, texture_size = static_cast<int>(m_texture_size), cascade_level = static_cast<int>(m_number_of_cascades)](render_system&) mutable
                    {
                        OPTICK_EVENT("Create directional shadow resources");
                        GLuint texture_id = 0;
                        gl_exec(glGenTextures, 1, &texture_id);
                        gl_exec(glBindTexture, GL_TEXTURE_2D_ARRAY, texture_id);
                        gl_exec(glTexImage3D, GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT32F, texture_size, texture_size, cascade_level, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
                        gl_exec(glTexParameteri, GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                        gl_exec(glTexParameteri, GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                        gl_exec(glTexParameteri, GL_TEXTURE_2D_ARRAY, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
                        gl_exec(glTexParameteri, GL_TEXTURE_2D_ARRAY, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
                        gl_exec(glTexParameteri, GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
                        gl_exec(glTexParameteri, GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
                        constexpr float bordercolor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
                        gl_exec(glTexParameterfv, GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, bordercolor);
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
            auto camera_matrices = context.read<in_camera_matrix_pin>();
            auto camera_projections = context.read<in_camera_matrix_projection_pin>();
            auto light_directions = context.read<in_light_direction_pin>();
            if (camera_matrices.empty() || camera_projections.empty() || light_directions.empty())
                return;

            float const c22 = camera_projections[0][2][2];
            float const c32 = camera_projections[0][3][2];
            float const near_z = c32 / (c22 - 1.f);
            float const far_z = c32 / (c22 + 1.f);
            float const fov_y = 2.0f * std::atan(1.0f / camera_projections[0][1][1]);
            float const aspect_ratio = camera_projections[0][1][1] / camera_projections[0][0][0];

            std::vector<float> cascade_distances;
            std::vector<glm::mat4> light_matrices;
            float previous_z = near_z;
            for (int i = 1; i <= m_number_of_cascades; ++i)
            {
                float const log_factor = near_z * std::pow(far_z / near_z, static_cast<float>(i)/m_number_of_cascades);
                float const linear_factor = near_z + ((static_cast<float>(i) / m_number_of_cascades) * (far_z - near_z));
                float const cascade_distance = m_log_linear_split_factor * log_factor + (1.f - m_log_linear_split_factor) * linear_factor;

                cascade_distances.push_back(cascade_distance);
                glm::mat4 const cascade_projection = glm::perspective(fov_y, aspect_ratio, previous_z, cascade_distances.back());
                std::array<glm::vec4, 8> const corners = get_camera_frustrum_corners(cascade_projection, glm::inverse(camera_matrices[0]));
                light_matrices.push_back(create_light_matrix(light_directions[0], corners));
                previous_z = cascade_distance;
            }

            render_sys->get().add_pre_light_work(
                [memory = memory, texture_size = static_cast<int>(m_texture_size), light_matrices](render_system& render_sys) mutable
                {
                    OPTICK_EVENT("Render directional shadow");
                    gl_exec(glBindFramebuffer, GL_FRAMEBUFFER, memory.m_framebuffer->get_fbo());
                    gl_exec(glViewport, 0, 0, texture_size, texture_size);
                    gl_exec(glClear, GL_DEPTH_BUFFER_BIT);
                    gl_exec(glCullFace, GL_FRONT);
                    render_sys.for_each_mesh_draw_request(
                        [&light_matrices](render_system::draw_request const& request) mutable
                        {
                            gcore::resource_handle<program> shadow_program = request.m_program_data->m_program->get_associated_program(program::associated_program::directional_shadow);
                            if (shadow_program.is_loaded())
                            {
                                shadow_program->activate();
                                program_uniform_state state = shadow_program->get_default_state();
                                state.copy_matching_uniform(request.m_program_data->m_state);
                                state.set_uniform("light_matrices[0]", gtl::span<glm::mat4 const>(light_matrices));
                                state.set_uniform("world_matrix", gtl::span(request.m_transform));
                                state.apply();
                                request.m_mesh->draw();
                            }
                        }
                    );
                    gl_exec(glCullFace, GL_BACK);
                    gl_exec(glBindFramebuffer, GL_FRAMEBUFFER, 0);
                }
            );
            
            context.write_values<out_texture_pin>(gcore::resource_handle<texture>(memory.m_texture));
            context.write_values<out_light_matrix_pin>(light_matrices);
            context.write_values<out_cascade_distance_pin>(cascade_distances);
        }
    }

    gcore::node::pin_descriptors render_directional_shadow_node::get_pin_descriptors() const
    {
        static gcore::pin_descriptor const inputs[] = {
            in_light_direction_pin::get("light direction"),
            in_camera_matrix_pin::get("camera matrix"),
            in_camera_matrix_projection_pin::get("camera projection"),
        };

        static gcore::pin_descriptor const outputs[] = {
            out_texture_pin::get("shadow texture"),
            out_light_matrix_pin::get("light matrices"),
            out_cascade_distance_pin::get("cascade distances"),
        };
        return node::pin_descriptors{inputs, outputs};
    }

    void render_directional_shadow_node::process(gserializer::serializer& serializer)
    {
        render_shadow_node_base::process(serializer);
        serializer.process("number of cascades", m_number_of_cascades);
        serializer.process("log linear split factor", m_log_linear_split_factor);
    }

    std::array<glm::vec4, 8> render_directional_shadow_node::get_camera_frustrum_corners(glm::mat4 const& projection, glm::mat4 const& view)
    {
        std::array<glm::vec4, 8> return_value;
        glm::mat4 const inv = glm::inverse(projection * view);
        for (int x = 0; x < 2; ++x)
            for (int y = 0; y < 2; ++y)
                for (int z = 0; z < 2; ++z)
                {
                    glm::vec4 const corner = inv * glm::vec4((2.f * x) - 1.f, (2.f * y) - 1.f, (2.f * z) - 1.f, 1.f);
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
        glm::mat4 const light_view_matrix = glm::lookAt(center + glm::normalize(light_direction), center, generate_valid_up(light_direction));

        glm::vec3 min_corner = glm::vec3(std::numeric_limits<float>::max());
        glm::vec3 max_corner = glm::vec3(std::numeric_limits<float>::lowest());
        for (glm::vec4 const& corner : corners)
        {
            glm::vec3 const light_space_corner = glm::vec3(light_view_matrix * corner);
            min_corner = glm::min(min_corner, light_space_corner);
            max_corner = glm::max(max_corner, light_space_corner);
        }
        min_corner.z -= 20.f;
        max_corner.z += 20.f;

        glm::mat4 const light_projection_matrix = glm::ortho(min_corner.x, max_corner.x, min_corner.y, max_corner.y, min_corner.z, max_corner.z);
        return light_projection_matrix * light_view_matrix;
    }
    
    void render_point_light_shadow_node::execute(gcore::node_context& context) const
    {
        if (auto* render_sys = context.get_in_context<std::reference_wrapper<render_system>>())
        {
            auto& memory = std::any_cast<state&>(context.m_node_memory);
            if (memory.m_texture->get_id() == 0 && memory.m_framebuffer->get_fbo() == 0)
            {
                render_sys->get().add_pre_light_work(
                    [memory = memory, texture_size = static_cast<int>(m_texture_size)](render_system&) mutable
                {
                    OPTICK_EVENT("Create point light shadow resources");
                    GLuint texture_id = 0;
                    gl_exec(glGenTextures, 1, &texture_id);
                    gl_exec(glBindTexture, GL_TEXTURE_CUBE_MAP, texture_id);
                    for (int i = 0; i < 6; ++i)
                    {
                        gl_exec(glTexImage2D, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, texture_size, texture_size, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
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

            auto matrixes = context.read<in_light_matrix_pin>();
            auto radii = context.read<in_radius_pin>();
            if (matrixes.empty() || radii.empty())
                return;

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

            glm::vec3 const light_pos = glm::vec3(matrixes[0][3]);
            std::vector<glm::mat4> light_matrices;
            glm::mat4 const light_projection = glm::perspective(gmath::radian(90.0_d).value(), 1.f, 0.1f, radii[0]);
            for (int i = 0; i < 6; ++i)
            {
                glm::mat4 const light_view_matrix = glm::lookAt(light_pos, light_pos + directions[i], up_vectors[i]);
                light_matrices.push_back(light_projection * light_view_matrix);
            }

            render_sys->get().add_pre_light_work(
                [memory, light_matrices = std::move(light_matrices), texture_size = static_cast<int>(m_texture_size)](render_system& render_sys) mutable
            {
                OPTICK_EVENT("Render point light shadow");
                gl_exec(glBindFramebuffer, GL_FRAMEBUFFER, memory.m_framebuffer->get_fbo());
                gl_exec(glViewport, 0, 0, texture_size, texture_size);
                gl_exec(glClear, GL_DEPTH_BUFFER_BIT);
                gl_exec(glCullFace, GL_FRONT);
                render_sys.for_each_mesh_draw_request(
                    [&light_matrices](render_system::draw_request const& request) mutable
                    {
                        gcore::resource_handle<program> shadow_program = request.m_program_data->m_program->get_associated_program(program::associated_program::point_light_shadow);
                        if (shadow_program.is_loaded())
                        {
                            shadow_program->activate();
                            program_uniform_state state = shadow_program->get_default_state();
                            state.copy_matching_uniform(request.m_program_data->m_state);
                            state.set_uniform("world_matrix", gtl::span(request.m_transform));
                            state.set_uniform("light_matrices[0]", gtl::span<glm::mat4 const>(light_matrices));
                            state.apply();
                            request.m_mesh->draw();
                        }
                    }
                );


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
            in_light_matrix_pin::get("light matrix"),
            in_radius_pin::get("radius"),
        };

        static gcore::pin_descriptor const outputs[] = {
            out_texture_pin::get("shadow texture"),
        };
        return node::pin_descriptors{ inputs, outputs };
    }

    void render_spot_light_shadow_node::execute(gcore::node_context& context) const
    {
        if (auto* render_sys = context.get_in_context<std::reference_wrapper<render_system>>())
        {
            auto& memory = std::any_cast<state&>(context.m_node_memory);
            if (memory.m_texture->get_id() == 0 && memory.m_framebuffer->get_fbo() == 0)
            {
                render_sys->get().add_pre_light_work(
                    [memory = memory, texture_size = static_cast<int>(m_texture_size)](render_system&) mutable
                    {
                        OPTICK_EVENT("Create spot light shadow resources");
                        GLuint texture_id = 0;
                        gl_exec(glGenTextures, 1, &texture_id);
                        gl_exec(glBindTexture, GL_TEXTURE_2D, texture_id);
                        gl_exec(glTexImage2D, GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, texture_size, texture_size, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
                        gl_exec(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                        gl_exec(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                        gl_exec(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
                        gl_exec(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
                        gl_exec(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
                        gl_exec(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
                        constexpr float bordercolor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
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

            auto matrixes = context.read<in_light_matrix_pin>();
            auto angles = context.read<in_angle_pin>();
            auto distances = context.read<in_distance_pin>();
            if (matrixes.empty() || angles.empty() || distances.empty())
                return;

            glm::mat4 const view_matrix = glm::inverse(matrixes[0]);
            glm::mat4 const light_projection = glm::perspective(glm::radians(angles[0]), 1.f, 0.1f, distances[0]);
            glm::mat4 const light_matrix = light_projection * view_matrix;

            render_sys->get().add_pre_light_work(
                [memory, light_matrix, texture_size = static_cast<int>(m_texture_size)](render_system& render_sys) mutable
            {
                OPTICK_EVENT("Render spot light shadow");
                gl_exec(glBindFramebuffer, GL_FRAMEBUFFER, memory.m_framebuffer->get_fbo());
                gl_exec(glViewport, 0, 0, texture_size, texture_size);
                gl_exec(glClear, GL_DEPTH_BUFFER_BIT);
                gl_exec(glCullFace, GL_FRONT);

                render_sys.for_each_mesh_draw_request(
                    [&light_matrix](render_system::draw_request const& request) mutable
                    {
                        gcore::resource_handle<program> shadow_program = request.m_program_data->m_program->get_associated_program(program::associated_program::spot_light_shadow);
                        if (shadow_program.is_loaded())
                        {
                            glm::mat4 const mvp = light_matrix * request.m_transform;
                            shadow_program->activate();
                            program_uniform_state state = shadow_program->get_default_state();
                            state.copy_matching_uniform(request.m_program_data->m_state);
                            state.set_uniform("mvp", gtl::span(mvp));
                            state.apply();
                            request.m_mesh->draw();
                        }
                    }
                );

                gl_exec(glCullFace, GL_BACK);
                gl_exec(glBindFramebuffer, GL_FRAMEBUFFER, 0);
            }
            );

            context.write_values<out_matrix_pin>(light_matrix);
            context.write_values<out_texture_pin>(gcore::resource_handle<texture>(memory.m_texture));
        }
    }

    gcore::node::pin_descriptors render_spot_light_shadow_node::get_pin_descriptors() const
    {
        static gcore::pin_descriptor const inputs[] = {
            in_light_matrix_pin::get("light world matrix"),
            in_angle_pin::get("angle"),
            in_distance_pin::get("distance"),
        };

        static gcore::pin_descriptor const outputs[] = {
            out_texture_pin::get("shadow texture"),
            out_matrix_pin::get("light matrix"),
        };
        return node::pin_descriptors{ inputs, outputs };
    }
}
