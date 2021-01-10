#include "stdafx.h"

#include <bullet/BulletCollision/CollisionShapes/btSphereShape.h>

#include "gphys/resources/sphere_shape.h"

#include "gserializer/serializer.h"

GSERIALIZER_DEFINE_SUBCLASS_FACTORY_REGISTRATION(gphys::sphere_shape);

namespace gphys
{
    void sphere_shape::process(gserializer::serializer& serializer)
    {
        collision_shape::process(serializer);
        serializer.process("radius", m_radius);
    }

    bool sphere_shape::do_load_async()
    {
        m_shape = std::make_shared<btSphereShape>(m_radius);
        return true;
    }
}