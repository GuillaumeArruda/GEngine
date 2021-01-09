#include "stdafx.h"
#include "gphys/resources/collision_shape.h"

#include <bullet/btBulletCollisionCommon.h>

namespace gphys
{
    collision_shape::~collision_shape() = default;
    collision_shape::collision_shape(collision_shape&&) = default;
    collision_shape& collision_shape::operator=(collision_shape&&) = default;
}
