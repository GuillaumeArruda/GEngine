#include <stdafx.h>

#include "gtool/imgui_serializer_override_libraby.h"

namespace gtool
{
    imgui_serializer_override_library& imgui_serializer_override_library::get()
    {
        static imgui_serializer_override_library instance;
        return instance;
    }

    imgui_serializer_override* imgui_serializer_override_library::get_override(std::type_index index)
    {
        if (auto it = m_overrides.find(index); it != m_overrides.end())
        {
            return &(*it).second;
        }
        return nullptr;
    }
}