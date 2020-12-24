#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <GL/glew.h>

#include "gcore/system.h"
#include "gcore/resource_handle.h"

#include "grender/resources/program.h"
#include "grender/resources/mesh_resource.h"

namespace gcore
{
    struct resource_library;
}

namespace grender
{
    struct debug_render_system : gcore::system
    {
        debug_render_system();
        ~debug_render_system();

        struct line
        {
            glm::vec3 m_start;
            glm::vec3 m_end;
        };

        struct aabb
        {
            glm::vec3 m_min;
            glm::vec3 m_max;
        };

        void init_resources(gcore::resource_library& library);

        void draw_line(glm::vec3 start, glm::vec3 end, glm::vec3 color = glm::vec3(1.f));

        void draw_primitives(glm::mat4 const& projection, glm::mat4 const& view);

    private:
        std::vector<line> m_line_to_draw;
        std::vector<glm::vec3> m_line_color;

        gcore::resource_handle<program> m_program;
        program_uniform_state m_program_state;

        GLuint m_line_vao;
        GLuint m_line_vbos[2]; // vertex and color
    };
}