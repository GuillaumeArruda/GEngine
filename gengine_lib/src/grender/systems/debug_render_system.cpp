#include "stdafx.h"
#include "grender/systems/debug_render_system.h"

#include "gcore/resource_library.h"

#include "grender/utils.h"

namespace grender
{
    debug_render_system::debug_render_system()
    {
        gl_exec(glGenVertexArrays, 1, &m_line_vao);
        gl_exec(glBindVertexArray, m_line_vao);

        gl_exec(glGenBuffers, 2, m_line_vbos);
        // positions
        gl_exec(glBindBuffer, GL_ARRAY_BUFFER, m_line_vbos[0]);
        gl_exec(glVertexAttribPointer, 0, 3, GL_FLOAT, static_cast<GLboolean>(GL_FALSE), 0, nullptr);
        gl_exec(glEnableVertexAttribArray, 0);
        //colors
        gl_exec(glBindBuffer, GL_ARRAY_BUFFER, m_line_vbos[1]);
        gl_exec(glVertexAttribPointer, 1, 3, GL_FLOAT, static_cast<GLboolean>(GL_FALSE), 0, nullptr);
        gl_exec(glEnableVertexAttribArray, 1);

        gl_exec(glBindBuffer, GL_ARRAY_BUFFER, 0);
        gl_exec(glBindVertexArray, 0);
    }

    debug_render_system::~debug_render_system()
    {
        gl_exec(glDeleteBuffers, 2, m_line_vbos);
        gl_exec(glDeleteVertexArrays, 1, &m_line_vao);
    }


    void debug_render_system::init_resources(gcore::resource_library& library)
    {
        m_program = library.get_resource<program>("5fe3cac284fd2c73e312df4c0afd7e5c"_gtl_uuid);
    }

    void debug_render_system::draw_line(glm::vec3 start, glm::vec3 end, glm::vec3 color)
    {
        m_line_to_draw.push_back(line{ start, end });
        m_line_color.push_back(color);
        m_line_color.push_back(color);
    }

    void debug_render_system::draw_primitives(glm::mat4 const& projection, glm::mat4 const& view)
    {
        if (m_program.is_loaded() && m_line_to_draw.size() != 0)
        {
            m_program_state.reconcile(m_program->get_default_state());
            gl_exec(glBindVertexArray, m_line_vao);
            gl_exec(glBindBuffer, GL_ARRAY_BUFFER, m_line_vbos[0]);
            gl_exec(glBufferData, GL_ARRAY_BUFFER, m_line_to_draw.size() * sizeof(glm::vec3) * 2, m_line_to_draw.data(), GL_STREAM_DRAW);

            gl_exec(glBindBuffer, GL_ARRAY_BUFFER, m_line_vbos[1]);
            gl_exec(glBufferData, GL_ARRAY_BUFFER, m_line_color.size() * sizeof(glm::vec3), m_line_color.data(), GL_STREAM_DRAW);

            m_program->activate();
            m_program_state.set_uniform("mvp", projection * view);
            m_program_state.apply();
            gl_exec(glDrawArrays, GL_LINES, 0, static_cast<GLsizei>(m_line_to_draw.size() * 2));

            gl_exec(glBindVertexArray, 0);
            m_line_to_draw.clear();
            m_line_color.clear();
        }
    }
}
