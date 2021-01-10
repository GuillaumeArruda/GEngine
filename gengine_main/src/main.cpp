#include <IL/il.h>
#include <IL/ilu.h>

#include <FileWatch/FileWatch.h>

#include "gcore/world.h"
#include "gcore/resource_library.h"
#include "gcore/systems/input_system.h"
#include "gcore/systems/flying_controller_system.h"

#include "grender/systems/render_system.h"
#include "grender/systems/debug_render_system.h"

#include "gphys/systems/physic_systems.h"

#include "gtool/window_manager.h"

int main()
{
    {
        ilInit();
        iluInit();
        gtool::window_manager tool_window_manager;
        std::shared_ptr<gcore::resource_library> lib = std::make_shared<gcore::resource_library>(tool_window_manager.get_window());
        lib->scan_directory("data\\");
        gcore::world world(lib);
        auto& systems = world.get_system_registry();

        systems.add_system(std::make_unique<grender::render_system>());
        systems.add_system(std::make_unique<gcore::input_system>());
        systems.add_system(std::make_unique<gcore::flying_controller_system>());
        systems.add_system(std::make_unique<gphys::physic_system>());
        systems.add_system(std::make_unique<grender::debug_render_system>());

        world.initialize_systems();

        auto input_task = world.m_update_tasks.add_task([&] { world.get_system_registry().get_system<gcore::input_system>()->update(world); });
        auto flying_controller_task = world.m_update_tasks.add_task([&] { world.get_system_registry().get_system<gcore::flying_controller_system>()->update(world); });
        auto physic_system_task = world.m_update_tasks.add_task([&] { world.get_system_registry().get_system<gphys::physic_system>()->update(world); });
        world.m_update_tasks.add_parent(flying_controller_task, input_task);

        {

            filewatch::FileWatch<std::wstring> watcher(L"./",
                [&](std::wstring const& path, filewatch::Event const&)
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