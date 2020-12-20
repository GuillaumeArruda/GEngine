#pragma once

#include "gcore/system.h"
struct GLFWwindow;
namespace gcore
{
    struct world;
    struct input_system : system
    {
        void update(world& world);
        GLFWwindow* m_window = nullptr;
    };
}