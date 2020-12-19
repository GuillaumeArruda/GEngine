#include "stdafx.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "grender/systems/render_system.h"

#include "gcore/components/transform_component.h"
#include "gcore/entity_registry.h"
#include "gcore/resource_library.h"

#include "grender/components/camera_component.h"
#include "grender/components/graphic_component.h"
#include "grender/resources/mesh_resource.h"
#include "grender/resources/program.h"
#include "grender/utils.h"

namespace grender
{
    void render_system::render(gcore::entity_registry& registry, gcore::resource_library& library)
    {
        gl_exec(glClearColor, 0.0f, 0.0f, 0.0f, 1.0f);
        gl_exec(glClear, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        gl_exec(glEnable, GL_DEPTH_TEST);
       
        auto camera_view = registry.get_view<gcore::transform_component, camera_component>();
        auto graphic_comp_view = registry.get_view<gcore::transform_component, graphic_component>();
        for (auto& [camera_transform, camera] : camera_view)
        {
            if (camera->m_active)
            {
                glm::mat4 const projection = glm::perspective(static_cast<float>(gmath::radian(camera->m_fov)), 16.f / 9.f, camera->m_near_z, camera->m_far_z);
                glm::mat4 const view_matrix = static_cast<glm::mat4>(camera_transform->m_transform);
                for (auto& [transform, graphic_comp] : graphic_comp_view)
                {
                    glm::mat4 const mvp = projection * view_matrix * static_cast<glm::mat4>(transform->m_transform);
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
                                }
                            }
                        }

                        if (mesh_info.m_mesh && mesh_info.m_program)
                        {
                            auto const mvp_id = mesh_info.m_uniform_state.get_uniform_location("MVP");
                            mesh_info.m_program->activate();
                            mesh_info.m_uniform_state.set_uniform(mvp_id, mvp);
                            mesh_info.m_uniform_state.apply();
                            for (auto& submesh : mesh_info.m_mesh->get_meshes())
                            {
                                submesh.draw();
                            }
                        }
                    }
                }
            }
            
        }
       
    }
}

