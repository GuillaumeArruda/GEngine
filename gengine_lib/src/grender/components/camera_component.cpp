#include "stdafx.h"
#include "grender/components/camera_component.h"

#include "gserializer/serializer.h"
#include "gserializer/gmath_serialization.h"

GSERIALIZER_DEFINE_SUBCLASS_FACTORY_REGISTRATION(grender::camera_component);

namespace grender
{
    void camera_component::process(gserializer::serializer& serializer)
    {
        serializer.process("fov", m_fov);
        serializer.process("near_z", m_near_z);
        serializer.process("far_z", m_far_z);
        serializer.process("active", m_active);
    }
}

