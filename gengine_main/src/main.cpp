#include <vector>
#include <cassert>
#include <iostream>

#include "gcore/world.h"

#include "gcore/resource_library.h"
#include "gtool/systems/create_entity_systems.h"

#include "grender/systems/window_system.h"
#include "grender/systems/render_system.h"

int main()
{

    gcore::world world;
    auto& systems = world.get_system_registry();
    auto& registry = world.get_entity_registry();

    systems.add_system(std::make_unique<grender::window_system>("test", 1920, 1080));
    systems.add_system(std::make_unique<grender::render_system>());
    systems.add_system(std::make_unique<gtool::create_entity_system>());
    grender::window_system* window = systems.get_system<grender::window_system>();
    grender::render_system* render = systems.get_system<grender::render_system>();
    gtool::create_entity_system* create_entity = systems.get_system<gtool::create_entity_system>();
    
    window->bind_context();
    {
        gcore::resource_library lib;
        lib.scan_directory("data\\");
        while (window->should_close())
        {
            window->begin_frame();
            render->render(registry, lib);
            create_entity->update(registry);
            window->end_frame();
        }
    }

    return 0;
}