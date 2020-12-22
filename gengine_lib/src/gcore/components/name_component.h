#pragma once

#include "gcore/component.h"

namespace gcore
{
    struct name_component : component
    {
        void process(gserializer::serializer& serializer) override;
        std::string m_name;
        GSERIALIZER_DECLARE_SUBCLASS_FACTORY_REGISTRATION();
    };
};