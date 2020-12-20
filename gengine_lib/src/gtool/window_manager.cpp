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

#include "gtool/window.h"
#include "gtool/windows/entity_view_window.h"
#include "gtool/windows/resource_view_window.h"
#include "gtool/windows/viewport_window.h"


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

		m_window = glfwCreateWindow(mode->width, mode->height, "gengine", nullptr, nullptr);

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

		auto& systems = world.get_system_registry();
		auto& registry = world.get_entity_registry();
		auto lib = world.get_resource_library();

		grender::render_system* render = systems.get_system<grender::render_system>();
		gcore::input_system* input_system = systems.get_system<gcore::input_system>();
		gcore::flying_controller_system* controller_system = systems.get_system<gcore::flying_controller_system>();

		input_system->update(world);
		controller_system->update(world);
		render->render(registry, *lib);
		lib->update();

		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGuiWindowFlags window_flags = 
			ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking
			| ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse
			| ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove
			| ImGuiWindowFlags_NoBringToFrontOnFocus;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::Begin("Master DockSpace", NULL, window_flags);
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
