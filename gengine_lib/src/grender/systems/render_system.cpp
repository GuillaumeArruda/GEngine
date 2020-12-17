#include "stdafx.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "grender/systems/render_system.h"

#include "gcore/components/transform_component.h"
#include "gcore/entity_registry.h"
#include "gcore/resource_library.h"

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

        program* p = library.get_resource<program>("5fdb7534af7c1b239419bb136a2b00e7"_gtl_uuid);
        p->activate();
        auto const id = glGetUniformLocation(p->get_id(), "MVP");
        glm::mat4 const view_mat = glm::lookAt(glm::vec3(0.f, 200.f, 200.f), glm::vec3(0.f, 120.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
        glm::mat4 const projection = glm::perspective(glm::radians(70.f), 16.f / 9.f, 0.1f, 1000.f);

        auto view = registry.get_view<gcore::transform_component, graphic_component>();
        for (auto& [transform, graphic_comp] : view)
        {
            glm::mat4 const mvp = projection * view_mat * static_cast<glm::mat4>(transform->m_transform);
            grender::gl_exec(glUniformMatrix4fv, id, 1, GL_FALSE, glm::value_ptr(mvp));
            for (auto const& mesh_uuid : graphic_comp->m_meshes)
            {
                if (mesh_resource const* mesh = library.get_resource<mesh_resource>(mesh_uuid))
                {
                    for (auto const& submesh : mesh->get_meshes())
                    {
                        submesh.draw();
                    }
                }
            }
        }
    }
}

