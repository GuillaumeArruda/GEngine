#include <stdafx.h>

#include "gtool/register_imgui_overrides.h"
#include "gtool/imgui_serializer_override_libraby.h"
#include "gtool/imgui_overrides/core_script_imgui_overrides.h"

namespace gtool
{
    void register_imgui_overrides()
    {
        auto& library = imgui_serializer_override_library::get();
        library.add_override<gcore::node_data>(&gtool::display_node_data);
        library.add_override<gcore::get_variable_node>(&gtool::display_variable_node);
        library.add_override<gcore::set_variable_node>(&gtool::display_variable_node);
    }
}