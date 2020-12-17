#pragma once

#include "gtl/uuid.h"

#include "gcore/component.h"

namespace grender
{
    struct graphic_component : gcore::component
    {
        void process(gserializer::serializer& serializer) override;

        std::vector<gtl::uuid> m_meshes;
        GSERIALIZER_DECLARE_SUBCLASS_FACTORY_REGISTRATION();
    };
}