#include "stdafx.h"
#include "grender/systems/window_system.h"

#include "grender/utils.h"
#include <GLFW/glfw3.h>

#include <imgui.h>
#include "imgui_impl/imgui_impl_opengl3.h"
#include "imgui_impl/imgui_impl_glfw.h"

namespace grender
{
    window_system::window_system(std::string const& window_name, int x_resolution, int y_resolution)
    {
        glfwInit();
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
        glfwTerminate();
    }

    window_system& window_system::operator=(window_system&& move)
    {
        if (this == &move)
            return *this;

        glfwDestroyWindow(m_window);
        m_window = move.m_window;
        move.m_window = nullptr;
        return *this;
    }

    void window_system::bind_context()
    {
        glfwMakeContextCurrent(m_window);
        ImGui::CreateContext();
        ImGui::StyleColorsDark();

        ImGui_ImplGlfw_InitForOpenGL(m_window, true);
        ImGui_ImplOpenGL3_Init(nullptr);    
        gl_exec(glewInit);
    }

    bool window_system::should_close() const
    {
        return glfwWindowShouldClose(m_window) == 0;
    }

    void window_system::begin_frame()
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        glfwPollEvents();
    }

    void window_system::end_frame()
    {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(m_window);
    }
}


