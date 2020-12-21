


#include <IL/il.h>

#include <FileWatch/FileWatch.h>


#include "gcore/world.h"

#include "gcore/resource_library.h"

#include "gcore/systems/input_system.h"
#include "gcore/systems/flying_controller_system.h"

#include "grender/systems/render_system.h"

#include "gtool/window_manager.h"

int main()
{
    {
        ilInit();
        gtool::window_manager tool_window_manager;
        std::shared_ptr<gcore::resource_library> lib = std::make_shared<gcore::resource_library>(tool_window_manager.get_window());
        lib->scan_directory("data\\");
        gcore::world world(lib);
        auto& systems = world.get_system_registry();
        auto& registry = world.get_entity_registry();

        systems.add_system(std::make_unique<grender::render_system>());
        systems.add_system(std::make_unique<gcore::input_system>());
        systems.add_system(std::make_unique<gcore::flying_controller_system>());

        grender::render_system* render = systems.get_system<grender::render_system>();
        gcore::input_system* input_system = systems.get_system<gcore::input_system>();
        gcore::flying_controller_system* controller_system = systems.get_system<gcore::flying_controller_system>();

        {

            filewatch::FileWatch<std::wstring> watcher(L"./",
                [&](std::wstring const& path, filewatch::Event const& event)
            {
                lib->on_file_change(path);
            }
            );
            while (!tool_window_manager.wants_to_close())
            {
                tool_window_manager.update(world);
            }
        }
    }


    return 0;
}