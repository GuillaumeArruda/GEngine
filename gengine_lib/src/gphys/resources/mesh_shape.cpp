#include "stdafx.h"

#include <memory>

#include <bullet/btBulletCollisionCommon.h>

#include "gphys/resources/mesh_shape.h"

#include "gserializer/serializer.h"


GSERIALIZER_DEFINE_SUBCLASS_FACTORY_REGISTRATION(gphys::mesh_shape);

namespace gphys
{
    mesh_shape::~mesh_shape() = default;
    mesh_shape::mesh_shape(mesh_shape&&) noexcept = default;
    mesh_shape& mesh_shape::operator=(mesh_shape&&) = default;

    void mesh_shape::process(gserializer::serializer& serializer)
    {
        resource::process(serializer);
    }

    bool mesh_shape::do_load_async() 
    {
        m_shape = std::make_unique<btCapsuleShape>(0.5f, 0.8f);
        return true;
    }

    bool mesh_shape::do_load_sync()
    {
        return true;
    }

    void mesh_shape::do_unload()
    {
        m_shape.reset();
    }
}


