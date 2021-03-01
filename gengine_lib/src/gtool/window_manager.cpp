#include "stdafx.h"
#include <GL/glew.h>
#include <charconv>

#include "gtool/window_manager.h"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>

#include <optick/optick.h>

#include "gcore/world.h"
#include "gcore/resource_library.h"
#include "gcore/console.h"

#include "grender/systems/render_system.h"
#include "grender/utils.h"

#include "gphys/systems/physic_systems.h"

#include "gtool/window.h"
#include "gtool/windows/entity_view_window.h"
#include "gtool/windows/resource_view_window.h"
#include "gtool/windows/viewport_window.h"
#include "gtool/windows/node_editor_window.h"
#include "gtool/windows/console_window.h"
#include "gtool/windows/frame_view_window.h"


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

		gcore::console::get().register_callback("window.enable_vsync",
			[&](std::string_view value) {
			int swap_interval = 0;
			auto const results = std::from_chars(value.data(), value.data() + value.size(), swap_interval);
			if (results.ec == std::errc())
			{
				glfwSwapInterval(swap_interval);
				return true;
			}
			return false;
		});

        m_windows.push_back(std::make_unique<entity_view_window>());
        m_windows.push_back(std::make_unique<resource_view_window>());
		m_windows.push_back(std::make_unique<viewport_window>());
		m_windows.push_back(std::make_unique<node_editor_window>());
		m_windows.push_back(std::make_unique<console_window>());
		m_windows.push_back(std::make_unique<frame_view_window>());


    }

	window_manager::~window_manager()
	{
		glfwDestroyWindow(m_window);
		glfwTerminate();
	}

    void window_manager::update(gcore::world& world)
    {
		OPTICK_FRAME("Main Thread");
		{
			
			glfwPollEvents();
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			{
				OPTICK_EVENT("World Update");
				world.get_time().new_frame();
				world.m_update_tasks.run_tasks(m_jobs);
				grender::render_system* render = world.get_system_registry().get_system<grender::render_system>();
				render->update(world);
				world.get_resource_library()->update();
				world.get_entity_registry().update();
			}

			{
				OPTICK_EVENT("Tool Update");
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

				{
					OPTICK_EVENT("ImGui Render");
					ImGui::Render();
					ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
				}
				
			}
		}
		{
			OPTICK_EVENT("Swap Buffers");
			glfwSwapBuffers(m_window);
		}

    }

	bool window_manager::wants_to_close() const
	{
		return glfwWindowShouldClose(m_window) != 0;
	}

}
