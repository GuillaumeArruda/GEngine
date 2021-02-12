#include "stdafx.h"
#include "gcore/script/node.h"

#include "gserializer/serializer.h"

namespace gcore
{
    void node::process(gserializer::serializer& serializer)
    {
        serializer.process("node_id", m_node_id);
    }
}
