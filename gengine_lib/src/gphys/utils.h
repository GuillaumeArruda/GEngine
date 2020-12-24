#pragma once


#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>


#include <bullet/LinearMath/btQuaternion.h>
#include <bullet/LinearMath/btVector3.h>
#include <bullet/LinearMath/btTransform.h>

namespace gphys
{
    glm::mat4 to_glm(btQuaternion const& quaternion, btVector3 const& position);
    glm::mat4 to_glm(btTransform const& transform);
    glm::vec3 to_glm(btVector3 const& position);
    glm::quat to_glm(btQuaternion const& quaternion);

    btQuaternion to_bullet(glm::quat const& quaternion);
    btVector3 to_bullet(btVector3 const& position);
    btTransform to_bullet(glm::mat4 const& matrix);
}