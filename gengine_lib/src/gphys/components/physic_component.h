#pragma once

#include "gtl/uuid.h"

#include "gcore/component.h"
#include "gcore/resource_handle.h"

#include "gphys/resources/mesh_shape.h"
#include "gphys/motion_state.h"

class btRigidBody;

namespace gphys
{
    struct physic_component : gcore::component
    {
        void process(gserializer::serializer& serializer) override;

        physic_component() = default;
        ~physic_component();

        physic_component(physic_component&&) noexcept;
        physic_component& operator=(physic_component&&);

        physic_component(physic_component const&) = delete;
        physic_component operator=(physic_component const&) = delete;

        gtl::uuid m_phys_shape_uuid;
        gcore::resource_handle<mesh_shape> m_phys_shape;
        std::unique_ptr<btRigidBody> m_rigid_body;
        motion_state m_motion_state;
        GSERIALIZER_DECLARE_SUBCLASS_FACTORY_REGISTRATION();
    };
}