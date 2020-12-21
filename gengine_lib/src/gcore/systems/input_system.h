#pragma once

#include "gcore/system.h"
#include <glm/glm.hpp>


struct GLFWwindow;
namespace gcore
{
    struct world;
    struct input_system : system
    {
        void update(world& world);
        GLFWwindow* m_window = nullptr;
        glm::ivec2 m_start_point = {};
        glm::ivec2 m_viewport_size = {};
    };
}