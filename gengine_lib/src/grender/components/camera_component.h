#pragma once

#include "gmath/units.h"

#include "gcore/component.h"

namespace grender
{
    struct camera_component : gcore::component
    {
        void process(gserializer::serializer& serializer) override;

        gmath::degree m_fov = 70.0_d;
        float m_near_z = 0.1f;
        float m_far_z = 100.f;
        bool m_active = true;

        glm::mat4 create_perspective_matrix(float aspect) const;
        GSERIALIZER_DECLARE_SUBCLASS_FACTORY_REGISTRATION();
    };
}