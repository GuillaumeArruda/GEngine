#include "stdafx.h"

#include "gcore/resource.h"

#include "gserializer/serializer.h"

namespace gcore
{
    void resource::process(gserializer::serializer& serializer)
    {
        serializer.process("uuid", m_uuid);
        serializer.process("name", m_name);
    }
}

