#pragma once

#include "gcore/component.h"
#include "gcore/input_key.h"

#include "gmath/units.h"

namespace gcore
{
    struct flying_controller_component : component
    {
        void process(gserializer::serializer& serializer) override;

        float m_pitch = 0.f;
        float m_yaw = 0.f;
        float m_speed = 1.f;
        float m_boost_factor = 5.f;
        gmath::degree m_rotation_speed = 180.0_d;
        keyboard_key m_up = keyboard_key::x;
        keyboard_key m_down = keyboard_key::z;
        keyboard_key m_forward = keyboard_key::w;
        keyboard_key m_backward = keyboard_key::s;
        keyboard_key m_left = keyboard_key::a;
        keyboard_key m_right = keyboard_key::d;
        keyboard_key m_boost_key = keyboard_key::left_shift;
    private:
        GSERIALIZER_DECLARE_SUBCLASS_FACTORY_REGISTRATION();
    };
}