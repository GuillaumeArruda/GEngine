#pragma once

#include "gtl/uuid.h"

#include "gcore/component.h"
#include "gcore/resource_handle.h"
#include "gcore/script/script_context.h"

#include "grender/resources/graphic_script.h"
#include "gcore/script/script_runner.h"

namespace grender
{
    struct graphic_component : gcore::component
    {
        void process(gserializer::serializer& serializer) override;

        gcore::script_runner<graphic_script> m_script;

        GSERIALIZER_DECLARE_SUBCLASS_FACTORY_REGISTRATION();
    };
}