#include "stdafx.h"

#include <bullet/BulletCollision/CollisionShapes/btCapsuleShape.h>

#include "gphys/resources/capsule_shape.h"

#include "gserializer/serializer.h"

GSERIALIZER_DEFINE_SUBCLASS_FACTORY_REGISTRATION(gphys::capsule_shape);

namespace gphys
{
    void capsule_shape::process(gserializer::serializer& serializer)
    {
        collision_shape::process(serializer);
        serializer.process("half_height", m_half_height);
        serializer.process("radius", m_radius);
    }

    bool capsule_shape::do_load_async()
    {
        m_shape = std::make_shared<btCapsuleShape>(m_radius, m_half_height);
        return true;
    }

    bool capsule_shape::do_load_sync()
    {
        return true;
    }

    void capsule_shape::do_unload()
    {
        m_shape.reset();
    }

}

