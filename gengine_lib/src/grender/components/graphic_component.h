#pragma once

#include "gtl/uuid.h"

#include "gcore/component.h"
#include "gcore/resource_handle.h"
#include "gcore/script/script_context.h"

#include "grender/resources/program.h"
#include "grender/resources/mesh_resource.h"

#include "grender/resources/graphic_script.h"

namespace grender
{
    struct graphic_component : gcore::component
    {
        void process(gserializer::serializer& serializer) override;

        gcore::resource_handle<graphic_script> m_script;
        gcore::script_context m_script_context;
        
        GSERIALIZER_DECLARE_SUBCLASS_FACTORY_REGISTRATION();
    };
}