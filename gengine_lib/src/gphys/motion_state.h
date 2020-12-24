#pragma once

#include <bullet/LinearMath/btMotionState.h>

#include <glm/glm.hpp>

namespace gcore
{
    struct transform_component;
}

namespace gphys
{
    struct motion_state : btMotionState
    {
        void getWorldTransform(btTransform& transform) const override;
        void setWorldTransform(btTransform const& transform) override;
        glm::vec3 m_offset = glm::vec3(0.f);
        gcore::transform_component* m_transform = nullptr;
    };
}