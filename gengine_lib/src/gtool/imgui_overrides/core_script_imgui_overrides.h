#pragma once

#include "gcore/script/node_data.h"
#include "gcore/script/nodes/variable_nodes.h"

namespace gtool
{
    struct imgui_serializer;
    void display_node_data(imgui_serializer& serializer, void* address);
    void display_variable_node(imgui_serializer& serializer, void* address);
    void display_node_descriptor(imgui_serializer& serializer, void* address);
}