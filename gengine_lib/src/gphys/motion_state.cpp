#include "stdafx.h"

#include "gphys/motion_state.h"

#include "gcore/components/transform_component.h"

#include "gphys/utils.h"

namespace gphys
{
    void motion_state::getWorldTransform(btTransform& transform) const
    {
        if (m_transform)
            transform = to_bullet(glm::translate(m_transform->m_transform, m_offset));
    }

    void motion_state::setWorldTransform(btTransform const& transform)
    {
        if (m_transform)
            m_transform->m_transform = glm::translate(to_glm(transform), -m_offset);
    }

}
