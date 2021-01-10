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

        std::shared_ptr<btCollisionShape> const& get_shape() { return m_shape; }
    protected:
        std::shared_ptr<btCollisionShape> m_shape;
    };
}