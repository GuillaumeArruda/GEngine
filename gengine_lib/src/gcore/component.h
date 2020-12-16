#pragma once

#include "gserializer/type_factory.h"

namespace gserializer
{
    struct serializer;
}

namespace gcore
{
    struct component
    {
        virtual ~component() = 0 {};
        virtual void process(gserializer::serializer&) {}
        GSERIALIZER_DECLARE_FACTORY_BASE(component);
    };
}