#pragma once

#include "gphys/resources/collision_shape.h"

namespace gphys 
{
    struct capsule_shape : collision_shape
    {
        void process(gserializer::serializer& serializer) override;

        bool do_load_async() override;
        bool do_load_sync() override;
        void do_unload() override;
        float m_half_height = 1.f;
        float m_radius = 0.5f;
        GSERIALIZER_DECLARE_SUBCLASS_FACTORY_REGISTRATION();
    };
}