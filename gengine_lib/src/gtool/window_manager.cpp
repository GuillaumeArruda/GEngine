#include "stdafx.h"
#include <GL/glew.h>

#include "gtool/window_manager.h"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>

#include "gcore/world.h"
#include "gcore/systems/input_system.h"
#include "gcore/systems/flying_controller_system.h"

#include "grender/systems/render_system.h"
#include "grender/utils.h"

#include "gphys/systems/physic_systems.h"

#include "gtool/window.h"
#include "gtool/windows/entity_view_window.h"
#include "gtool/windows/resource_view_window.h"
#include "gtool/windows/viewport_window.h"
#include "gtool/windows/node_editor_window.h"


namespace gtool
{
    window_manager::window_manager()
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

		m_window = glfwCreateWindow(1920, 1080, "gengine", nullptr, nullptr);

		glfwMakeContextCurrent(m_window);
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
		ImGui::StyleColorsDark();

		ImGui_ImplGlfw_InitForOpenGL(m_window, true);
		ImGui_ImplOpenGL3_Init(nullptr);
		grender::gl_exec(glewInit);

        m_windows.push_back(std::make_unique<entity_view_window>());
        m_windows.push_back(std::make_unique<resource_view_window>());
		m_windows.push_back(std::make_unique<viewport_window>());
		m_windows.push_back(std::make_unique<node_editor_window>());
    }

	window_manager::~window_manager()
	{
		glfwDestroyWindow(m_window);
		glfwTerminate();
	}

    void window_manager::update(gcore::world& world)
    {
		glfwPollEvents();
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		grender::render_system* render = world.get_system_registry().get_system<grender::render_system>();

		world.m_update_tasks.run_tasks(m_jobs);
		render->render(world);
		world.get_resource_library()->update();

		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGuiWindowFlags const window_flags = 
			ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking
			| ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse
			| ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove
			| ImGuiWindowFlags_NoBringToFrontOnFocus;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		if (ImGui::Begin("Master DockSpace", NULL, window_flags))
		{
			ImGuiID dockMain = ImGui::GetID("MyDockspace");

			ImGui::DockSpace(dockMain);
			if (ImGui::BeginMenuBar())
			{
				if (ImGui::BeginMenu("Windows"))
				{
					for (std::unique_ptr<window>& window : m_windows)
					{
						if (ImGui::MenuItem(window->get_name()))
						{
							window->get_should_display() = true;
						}
					}
					ImGui::EndMenu();
				}
				ImGui::EndMenuBar();
			}

			for (std::unique_ptr<window>& window : m_windows)
			{
				window->update(world, *this);
			}
		}

		ImGui::End();
		ImGui::PopStyleVar(3);

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(m_window);
    }

	bool window_manager::wants_to_close() const
	{
		return glfwWindowShouldClose(m_window) != 0;
	}

}
