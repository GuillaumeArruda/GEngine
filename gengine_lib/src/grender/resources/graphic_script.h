#pragma once

#include "gcore/script/script.h"

namespace grender 
{
    struct graphic_script : gcore::script
    {
        GSERIALIZER_DECLARE_SUBCLASS_FACTORY_REGISTRATION();

        ~graphic_script();
    };
}


