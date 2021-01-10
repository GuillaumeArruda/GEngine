#pragma once

#include "gphys/resources/collision_shape.h"

namespace gphys
{
    struct sphere_shape : collision_shape
    {
        void process(gserializer::serializer& serializer) override;
        bool do_load_async() override;
        bool do_load_sync() override { return true; }
        void do_unload() override { m_shape.reset(); }
        float m_radius = 1.f;
        GSERIALIZER_DECLARE_SUBCLASS_FACTORY_REGISTRATION();
    };
}