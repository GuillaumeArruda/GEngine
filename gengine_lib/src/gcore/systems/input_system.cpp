#include "stdafx.h"

#include "gcore/systems/input_system.h"

#include "gcore/components/input_component.h"
#include "gcore/world.h"

#include<GLFW/glfw3.h>

namespace gcore
{
    int keyboard_key_to_glfw_int(keyboard_key key);
    int mouse_key_to_glfw_int(mouse_key key);

    void input_system::update(world& world)
    {
        if (!m_window)
            return;
        
        input_component::keybord_state keybord_state{};

        for (int i = gtl::to_underlying(keyboard_key::first); i < gtl::to_underlying(keyboard_key::count); ++i)
        {
            auto key_state = glfwGetKey(m_window, keyboard_key_to_glfw_int(static_cast<keyboard_key>(i)));
            keybord_state[i] = key_state == GLFW_PRESS ? key_state::pressed : key_state::released;
        }

        input_component::mouse_key_state mouse_key_state{};
        for (int i = gtl::to_underlying(mouse_key::first); i < gtl::to_underlying(mouse_key::count); ++i)
        {
            auto key_state = glfwGetMouseButton (m_window, mouse_key_to_glfw_int(static_cast<mouse_key>(i)));
            mouse_key_state[i] = key_state == GLFW_PRESS ? key_state::pressed : key_state::released;
        }
        glm::dvec2 mouse_pos;
        glfwGetCursorPos(m_window, &mouse_pos[0], &mouse_pos[1]);
        if (m_viewport_size[0] == 0.f || m_viewport_size[1] == 0.f)
        {
            glfwGetWindowSize(m_window, &m_viewport_size[0], &m_viewport_size[1]);
        }

        mouse_pos -= m_start_point;
        mouse_pos /= m_viewport_size;

        auto view = world.get_entity_registry().get_view<input_component>();
        for (auto& [entity, input_component] : view)
        {
            input_component->m_keybord_state = keybord_state;
            input_component->m_mouse_key_state = mouse_key_state;
            input_component->m_mouse_positions[1] = input_component->m_mouse_positions[0];
            input_component->m_mouse_positions[0] = mouse_pos;
        }
    }

    int keyboard_key_to_glfw_int(keyboard_key key)
    {
        static constexpr std::array<int, gtl::to_underlying(keyboard_key::count)> glfw_map_key =
        {
            GLFW_KEY_SPACE,                     
            GLFW_KEY_APOSTROPHE,
            GLFW_KEY_COMMA,
            GLFW_KEY_MINUS,
            GLFW_KEY_PERIOD,
            GLFW_KEY_SLASH,
            GLFW_KEY_0,
            GLFW_KEY_1,
            GLFW_KEY_2,
            GLFW_KEY_3,
            GLFW_KEY_4,
            GLFW_KEY_5,
            GLFW_KEY_6,
            GLFW_KEY_7,
            GLFW_KEY_8,
            GLFW_KEY_9,
            GLFW_KEY_SEMICOLON,
            GLFW_KEY_EQUAL,
            GLFW_KEY_A,
            GLFW_KEY_B,
            GLFW_KEY_C,
            GLFW_KEY_D,
            GLFW_KEY_E,
            GLFW_KEY_F,
            GLFW_KEY_G,
            GLFW_KEY_H,
            GLFW_KEY_I,
            GLFW_KEY_J,
            GLFW_KEY_K,
            GLFW_KEY_L,
            GLFW_KEY_M,
            GLFW_KEY_N,
            GLFW_KEY_O,
            GLFW_KEY_P,
            GLFW_KEY_Q,
            GLFW_KEY_R,
            GLFW_KEY_S,
            GLFW_KEY_T,
            GLFW_KEY_U,
            GLFW_KEY_V,
            GLFW_KEY_W,
            GLFW_KEY_X,
            GLFW_KEY_Y,
            GLFW_KEY_Z,
            GLFW_KEY_LEFT_BRACKET,
            GLFW_KEY_BACKSLASH,
            GLFW_KEY_RIGHT_BRACKET,
            GLFW_KEY_GRAVE_ACCENT,
            GLFW_KEY_WORLD_1,
            GLFW_KEY_WORLD_2,
            GLFW_KEY_ESCAPE,
            GLFW_KEY_ENTER,
            GLFW_KEY_TAB,
            GLFW_KEY_BACKSPACE,
            GLFW_KEY_INSERT,
            GLFW_KEY_DELETE,
            GLFW_KEY_RIGHT,
            GLFW_KEY_LEFT,
            GLFW_KEY_DOWN,
            GLFW_KEY_UP,
            GLFW_KEY_PAGE_UP,
            GLFW_KEY_PAGE_DOWN,
            GLFW_KEY_HOME,
            GLFW_KEY_END,
            GLFW_KEY_CAPS_LOCK,
            GLFW_KEY_SCROLL_LOCK,
            GLFW_KEY_NUM_LOCK,
            GLFW_KEY_PRINT_SCREEN,
            GLFW_KEY_PAUSE,
            GLFW_KEY_F1,
            GLFW_KEY_F2,
            GLFW_KEY_F3,
            GLFW_KEY_F4,
            GLFW_KEY_F5,
            GLFW_KEY_F6,
            GLFW_KEY_F7,
            GLFW_KEY_F8,
            GLFW_KEY_F9,
            GLFW_KEY_F10,
            GLFW_KEY_F11,
            GLFW_KEY_F12,
            GLFW_KEY_F13,
            GLFW_KEY_F14,
            GLFW_KEY_F15,
            GLFW_KEY_F16,
            GLFW_KEY_F17,
            GLFW_KEY_F18,
            GLFW_KEY_F19,
            GLFW_KEY_F20,
            GLFW_KEY_F21,
            GLFW_KEY_F22,
            GLFW_KEY_F23,
            GLFW_KEY_F24,
            GLFW_KEY_F25,
            GLFW_KEY_KP_0,
            GLFW_KEY_KP_1,
            GLFW_KEY_KP_2,
            GLFW_KEY_KP_3,
            GLFW_KEY_KP_4,
            GLFW_KEY_KP_5,
            GLFW_KEY_KP_6,
            GLFW_KEY_KP_7,
            GLFW_KEY_KP_8,
            GLFW_KEY_KP_9,
            GLFW_KEY_KP_DECIMAL,
            GLFW_KEY_KP_DIVIDE,
            GLFW_KEY_KP_MULTIPLY,
            GLFW_KEY_KP_SUBTRACT,
            GLFW_KEY_KP_ADD,
            GLFW_KEY_KP_ENTER,
            GLFW_KEY_KP_EQUAL,
            GLFW_KEY_LEFT_SHIFT,
            GLFW_KEY_LEFT_CONTROL,
            GLFW_KEY_LEFT_ALT,
            GLFW_KEY_LEFT_SUPER,
            GLFW_KEY_RIGHT_SHIFT,
            GLFW_KEY_RIGHT_CONTROL,
            GLFW_KEY_RIGHT_ALT,
            GLFW_KEY_RIGHT_SUPER,
            GLFW_KEY_MENU,
        };
        return glfw_map_key[gtl::to_underlying(key)];
    }

    int mouse_key_to_glfw_int(mouse_key key)
    {
        static constexpr std::array<int, gtl::to_underlying(mouse_key::count)> glfw_map_key =
        {
            GLFW_MOUSE_BUTTON_1,
            GLFW_MOUSE_BUTTON_2,
            GLFW_MOUSE_BUTTON_3,
            GLFW_MOUSE_BUTTON_4,
            GLFW_MOUSE_BUTTON_5,
            GLFW_MOUSE_BUTTON_6,
            GLFW_MOUSE_BUTTON_7,
            GLFW_MOUSE_BUTTON_8,
        };
        return glfw_map_key[gtl::to_underlying(key)];
    }
       
}

