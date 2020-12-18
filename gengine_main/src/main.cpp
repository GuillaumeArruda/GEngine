#include <vector>
#include <cassert>
#include <iostream>

#include "gcore/world.h"

#include "gcore/resource_library.h"
#include "gtool/systems/entity_view_system.h"
#include "gtool/systems/resource_view_system.h"

#include "grender/systems/window_system.h"
#include "grender/systems/render_system.h"

int main()
{
    gcore::world world;
    auto& systems = world.get_system_registry();
    auto& registry = world.get_entity_registry();

    systems.add_system(std::make_unique<grender::window_system>("test", 1920, 1080));
    systems.add_system(std::make_unique<grender::render_system>());
    systems.add_system(std::make_unique<gtool::entity_view_system>());
    systems.add_system(std::make_unique<gtool::resouce_view_system>());
    grender::window_system* window = systems.get_system<grender::window_system>();
    grender::render_system* render = systems.get_system<grender::render_system>();
    gtool::entity_view_system* entity_view_system = systems.get_system<gtool::entity_view_system>();
    gtool::resouce_view_system* resource_view_system = systems.get_system<gtool::resouce_view_system>();
    
    window->bind_context();
    {
        gcore::resource_library lib;
        lib.scan_directory("data\\");
        while (window->should_close())
        {
            window->begin_frame();
            render->render(registry, lib);
            entity_view_system->update(registry);
            resource_view_system->update(lib);
            window->end_frame();
        }
    }

    return 0;
}