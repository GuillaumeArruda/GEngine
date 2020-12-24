#include "stdafx.h"
#include "gphys/debug_drawer.h"

#include <iostream>

#include "grender/systems/debug_render_system.h"

#include "gphys/utils.h"

namespace gphys
{
    void debug_drawer::reportErrorWarning(const char* warningString)
    {
        std::cerr << warningString;
    }

    void debug_drawer::drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
    {
        if (m_debug_render)
            m_debug_render->draw_line(to_glm(from), to_glm(to), to_glm(color));
    }
}
