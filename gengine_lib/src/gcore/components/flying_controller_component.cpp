#include "stdafx.h"
#include "gcore/components/flying_controller_component.h"

#include "gserializer/serializer.h"
#include "gserializer/gmath_serialization.h"

GSERIALIZER_DEFINE_SUBCLASS_FACTORY_REGISTRATION(gcore::flying_controller_component);

namespace gcore
{
    void flying_controller_component::process(gserializer::serializer& serializer)
    {
        serializer.process("speed", m_speed);
        serializer.process("boost_factor", m_boost_factor);
        serializer.process("rotation_speed", m_rotation_speed);
        serializer.process("up", m_up);
        serializer.process("down", m_down);
        serializer.process("forward", m_forward);
        serializer.process("backward", m_backward);
        serializer.process("left", m_left);
        serializer.process("right", m_right);
        serializer.process("boost_key", m_boost_key);
    }
}


