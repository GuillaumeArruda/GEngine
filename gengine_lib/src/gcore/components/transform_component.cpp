#include "stdafx.h"
#include "gcore/components/transform_component.h"

#include "gserializer/serializer.h"
#include "gserializer/gmath_serialization.h"

GSERIALIZER_DEFINE_SUBCLASS_FACTORY_REGISTRATION(gcore::transform_component);

namespace gcore
{
    void transform_component::process(gserializer::serializer& serializer)
    {
        serializer.process("transform", m_transform);
    }
}

