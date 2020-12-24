#include "stdafx.h"
#include "gphys/utils.h"

#include <glm/gtc/type_ptr.hpp>

namespace gphys
{
    glm::mat4 to_glm(btQuaternion const& quaternion, btVector3 const& position)
    {
        return  glm::translate(glm::mat4(1), to_glm(position)) * glm::mat4_cast(to_glm(quaternion));
    }

    glm::mat4 to_glm(btTransform const& transform)
    {
        glm::mat4 m;
        transform.getOpenGLMatrix(glm::value_ptr(m));
        return m;
    }

    glm::vec3 to_glm(btVector3 const& position)
    {
        return glm::vec3(position[0], position[1], position[2]);
    }

    glm::quat to_glm(btQuaternion const& quaternion)
    {
        return glm::quat(quaternion.getW(), quaternion.getX(), quaternion.getY(), quaternion.getZ());
    }

    btQuaternion to_bullet(glm::quat const& quaternion)
    {
        return btQuaternion(quaternion.x, quaternion.y, quaternion.z, quaternion.w);
    }

    btVector3 to_bullet(glm::vec3 const& position)
    {
        return btVector3(position[0], position[1], position[2]);
    }

    btTransform to_bullet(glm::mat4 const& matrix)
    {
        btTransform t;
        t.setFromOpenGLMatrix(glm::value_ptr(matrix));
        return t;
    }

}

