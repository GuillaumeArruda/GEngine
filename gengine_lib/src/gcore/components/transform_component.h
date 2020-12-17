#pragma once

#include "gmath/Transform.h"

#include "gcore/spaces.h"
#include "gcore/component.h"

namespace gcore
{
    struct transform_component : component
    {
        void process(gserializer::serializer& serializer) override;
        gmath::transform<gcore::world_space, gcore::model_space> m_transform{};
    private:
        GSERIALIZER_DECLARE_SUBCLASS_FACTORY_REGISTRATION();
    };
}