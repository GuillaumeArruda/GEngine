#include <stdafx.h>

#include "gcore/script/script_runner.h"
#include "gserializer/serializer.h"

namespace gcore
{
    void script_runner_base::input::process(gserializer::serializer& serializer)
    {
        serializer.process("variable_id", m_variable_id);
        serializer.process("value", m_value);
    }
}