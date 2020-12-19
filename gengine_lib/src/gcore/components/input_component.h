#pragma once

#include <array>

#include <glm/glm.hpp>

#include "gtl/utility.h"

#include "gcore/component.h"
#include "gcore/input_key.h"


namespace gcore
{
    struct input_component : component
    {
        using keybord_state = std::array<key_state, gtl::to_underlying(keyboard_key::count)>;
        using mouse_key_state = std::array < key_state, gtl::to_underlying(mouse_key::count)> ;

        keybord_state m_keybord_state;
        mouse_key_state m_mouse_key_state;
        std::array<glm::dvec2, 2> m_mouse_positions;
        void process(gserializer::serializer& serializer);
        GSERIALIZER_DECLARE_SUBCLASS_FACTORY_REGISTRATION();
    };
}