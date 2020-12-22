#include "stdafx.h"
#include "gcore/components/name_component.h"

#include "gserializer/serializer.h"

GSERIALIZER_DEFINE_SUBCLASS_FACTORY_REGISTRATION(gcore::name_component);

namespace gcore
{
    void name_component::process(gserializer::serializer& serializer)
    {
        serializer.process("name", m_name);
    }
}

