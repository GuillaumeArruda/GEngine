#include <vector>
#include <cassert>
#include <iostream>

#include "gcore/world.h"

#include "gcore/systems/window_system.h"

#include "gserializer/serializers/json_serializer.h"

#include <imgui.h>

struct component2 : gcore::component {
    ~component2() { std::cout << "component2\n"; }
    GSERIALIZER_DECLARE_SUBCLASS_FACTORY_REGISTRATION();
};

struct component3 : gcore::component 
{
    ~component3() { std::cout << "component3\n"; }
    GSERIALIZER_DECLARE_SUBCLASS_FACTORY_REGISTRATION();
};

GSERIALIZER_DEFINE_SUBCLASS_FACTORY_REGISTRATION(component2);
GSERIALIZER_DEFINE_SUBCLASS_FACTORY_REGISTRATION(component3);

#include <GLFW\glfw3.h>


int main()
{
    glfwInit();

    gcore::world world;
    auto& systems = world.get_system_registry();
    auto& registry = world.get_entity_registry();
    
    systems.add_system(std::make_unique<gcore::window_system>("test", 1280, 720));
    gcore::window_system* window = systems.get_system<gcore::window_system>();

    {
        gserializer::json_read_serializer seri("entities.json");
        seri.process("entity_registry", registry);
    }

    {
        gserializer::json_write_serializer seri;
        seri.process("entity_registry", registry);
        seri.write_to_file("entities.json");
    }

    auto view = registry.get_view<component2, component3>();
    for (auto& [comp2, comp3] : view)
    {
        std::cout << "Allo\n";
    }

    while (window->should_close())
    {
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}