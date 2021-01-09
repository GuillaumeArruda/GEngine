#pragma once

#include "gcore/resource.h"

class btCollisionShape;

namespace gphys
{
    struct collision_shape : gcore::resource
    {
        collision_shape() = default;
        ~collision_shape();

        collision_shape(collision_shape&&);
        collision_shape& operator=(collision_shape&&);

        collision_shape(collision_shape const&) = delete;
        collision_shape& operator=(collision_shape const&) = delete;

        btCollisionShape* get_shape() { return m_shape.get(); }
    protected:
        std::unique_ptr<btCollisionShape> m_shape;
    };
}