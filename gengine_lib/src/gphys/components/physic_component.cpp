#include "stdafx.h"

#include "gphys/components/physic_component.h"

#include "gserializer/serializer.h"

#include <bullet/btBulletDynamicsCommon.h>

GSERIALIZER_DEFINE_SUBCLASS_FACTORY_REGISTRATION(gphys::physic_component);

namespace gphys
{
    void physic_component::process(gserializer::serializer& serializer)
    {
        serializer.process("phys_shape", m_phys_shape_uuid, m_phys_shape);
    }

    physic_component::~physic_component() = default;
    physic_component::physic_component(physic_component&&) noexcept = default;
    physic_component& physic_component::operator=(physic_component&&) = default;
}
