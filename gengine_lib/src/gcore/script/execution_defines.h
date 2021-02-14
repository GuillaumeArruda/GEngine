#pragma once

namespace gserializer
{
    struct serializer;
}

namespace gcore
{
    struct execution_pin_data { void process(gserializer::serializer&) {} };
}