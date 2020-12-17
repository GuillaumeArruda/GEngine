#include "stdafx.h"
#include "window_system.h"

#include <GLFW/glfw3.h>

namespace gcore
{
    window_system::window_system(std::string const& window_name, int x_resolution, int y_resolution)
    {
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);

        glfwWindowHint(GLFW_RED_BITS, mode->redBits);
        glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
        glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
        glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

        m_window = glfwCreateWindow(x_resolution, y_resolution, window_name.c_str(), nullptr, nullptr);
    }

    window_system::~window_system()
    {
        glfwDestroyWindow(m_window);
    }

    window_system& window_system::operator=(window_system&& move)
    {
        if (this == &move)
            return *this;

        glfwDestroyWindow(m_window);
        m_window = move.m_window;
        move.m_window = nullptr;
    }

    void window_system::bind_context()
    {
        glfwMakeContextCurrent(m_window);
    }

    bool window_system::should_close() const
    {
        return glfwWindowShouldClose(m_window) == 0;
    }
}


