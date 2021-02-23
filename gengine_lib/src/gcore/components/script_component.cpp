#include "stdafx.h"
#include "gcore/components/script_component.h"


GSERIALIZER_DEFINE_SUBCLASS_FACTORY_REGISTRATION(gcore::script_component);

namespace gcore
{
    void script_component::process(gserializer::serializer& serializer)
    {
        component::process(serializer);
        serializer.process("scripts", m_scripts, "script");
    }

    void script_component::script_info::process(gserializer::serializer& serializer)
    {
        serializer.process("script", m_script);
    }
}

