#pragma once

#include <functional>
#include <unordered_map>

namespace gtool
{
    struct imgui_serializer;

    using imgui_serializer_override = std::move_only_function<void(imgui_serializer& serializer, void* address)>;

    struct imgui_serializer_override_library
    {
        static imgui_serializer_override_library& get();

        template<class TypeToOverride, class Function>
        void add_override(Function&& f)
        {
            m_overrides.emplace(std::type_index(typeid(TypeToOverride)), std::move(f));
        }

        imgui_serializer_override* get_override(std::type_index index);
    protected:
        std::unordered_map<std::type_index, imgui_serializer_override> m_overrides;
    };
}