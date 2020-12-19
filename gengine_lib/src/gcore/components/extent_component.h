#pragma once

#include "gcore/component.h"

#include "gmath/axis_aligned_box.h"

#include "gcore/spaces.h"


namespace gcore
{
    struct extent_component : component
    {
        gmath::axis_aligned_box<model_space> m_extent;
        void process(gserializer::serializer& serializer) override;
    private:
        GSERIALIZER_DECLARE_SUBCLASS_FACTORY_REGISTRATION();
    };
}