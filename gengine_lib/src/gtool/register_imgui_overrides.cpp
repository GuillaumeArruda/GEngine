#include <stdafx.h>

#include "gtool/register_imgui_overrides.h"
#include "gtool/imgui_serializer_override_libraby.h"
#include "gtool/imgui_overrides/core_script_imgui_overrides.h"

#include "grender/resources/graphic_script.h"

namespace gtool
{
    void register_imgui_overrides()
    {
        auto& library = imgui_serializer_override_library::get();
        library.add_override<gcore::node_data>(&gtool::display_node_data);
        library.add_override<gcore::get_variable_node>(&gtool::display_variable_node);
        library.add_override<gcore::set_variable_node>(&gtool::display_variable_node);
        library.add_override<gcore::script_descriptor::node_descriptor>(&gtool::display_node_descriptor);
        library.add_override<gcore::script_runner<grender::graphic_script>>(&gtool::display_script_runner<grender::graphic_script>);
    }
}