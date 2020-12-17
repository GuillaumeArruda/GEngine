#include <vector>
#include <cassert>
#include <iostream>

#include "gcore/world.h"

#include "gcore/systems/window_system.h"
#include "gcore/resource_library.h"

#include "grender/resources/mesh_resource.h"
#include "grender/resources/program.h"
#include "grender/utils.h"

#include "gserializer/serializers/json_serializer.h"

#include <imgui.h>
#include "imgui_impl/imgui_impl_opengl3.h"
#include "imgui_impl/imgui_impl_glfw.h"

#include <GLFW/glfw3.h>

#include <imgui.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

int main()
{
    glfwInit();

    gcore::world world;
    auto& systems = world.get_system_registry();
    auto& registry = world.get_entity_registry();

    systems.add_system(std::make_unique<gcore::window_system>("test", 1920, 1080));
    gcore::window_system* window = systems.get_system<gcore::window_system>();
    window->bind_context();
    grender::gl_exec(glewInit);
    
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    
    ImGui_ImplGlfw_InitForOpenGL(window->get_window(), true);
    ImGui_ImplOpenGL3_Init(nullptr);

    gserializer::json_write_serializer seri;
    {
        gcore::resource_library lib;
        lib.scan_directory("data\\");
        auto mesh = lib.get_resource<grender::mesh_resource>(gtl::uuid::from_string("5fdad25e7cad6150dd9c58c2db34a5ef"));
        auto program = lib.get_resource<grender::program>(gtl::uuid::from_string("5fdb7534af7c1b239419bb136a2b00e7"));

        glm::mat4 const model = glm::mat4(1.f);
        glm::mat4 const view = glm::lookAt(glm::vec3(0.f, 200.f, 200.f), glm::vec3(0.f, 120.f, 0.f), glm::vec3(0.f,1.f, 0.f));
        glm::mat4 const projection = glm::perspective(glm::radians(70.f), 16.f / 9.f, 0.1f, 1000.f);
        glm::mat4 const mvp = projection * view * model;

        auto const id = glGetUniformLocation(program->get_id(), "MVP");
       
        while (window->should_close())
        {
            glfwPollEvents();
            grender::gl_exec(glClearColor, 0.0f, 0.0f, 0.0f, 1.0f);
            grender::gl_exec(glClear, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            grender::gl_exec(glEnable, GL_DEPTH_TEST);
            program->activate();
            grender::gl_exec(glUniformMatrix4fv, id, 1, GL_FALSE, glm::value_ptr(mvp));
            for (auto& mesh : mesh->get_meshes())
            {
                mesh.draw();
            }
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            ImGui::ShowDemoWindow();
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            window->swap_buffers();
        }
        seri.process("resource_library", lib);
        seri.write_to_file("data\\resource_index.json");
    }

    glfwTerminate();
    return 0;
}