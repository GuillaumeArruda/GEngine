#include <vector>
#include <cassert>
#include <iostream>

#include "gcore/world.h"

#include "gcore/resource_library.h"

#include "gcore/systems/input_system.h"
#include "gcore/systems/flying_controller_system.h"

#include "gtool/systems/entity_view_system.h"
#include "gtool/systems/resource_view_system.h"

#include "grender/systems/window_system.h"
#include "grender/systems/render_system.h"

#include <IL/il.h>

int main()
{
    ilInit();
    gcore::world world;
    auto& systems = world.get_system_registry();
    auto& registry = world.get_entity_registry();

    systems.add_system(std::make_unique<grender::window_system>("test", 1920, 1080));
    systems.add_system(std::make_unique<grender::render_system>());
    systems.add_system(std::make_unique<gtool::entity_view_system>());
    systems.add_system(std::make_unique<gtool::resouce_view_system>());
    systems.add_system(std::make_unique<gcore::input_system>());
    systems.add_system(std::make_unique<gcore::flying_controller_system>());

    grender::window_system* window = systems.get_system<grender::window_system>();
    grender::render_system* render = systems.get_system<grender::render_system>();
    gtool::entity_view_system* entity_view_system = systems.get_system<gtool::entity_view_system>();
    gtool::resouce_view_system* resource_view_system = systems.get_system<gtool::resouce_view_system>();
    gcore::input_system* input_system = systems.get_system<gcore::input_system>();
    gcore::flying_controller_system* controller_system = systems.get_system<gcore::flying_controller_system>();

    window->bind_context();
    {
        gcore::resource_library lib;
        lib.scan_directory("data\\");
        while (window->should_close())
        {
            window->begin_frame();
            input_system->update(world);
            controller_system->update(world);
            render->render(registry, lib);
            entity_view_system->update(registry);
            resource_view_system->update(lib);
            window->end_frame();
        }
    }

    return 0;
}