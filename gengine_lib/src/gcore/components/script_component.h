#pragma once

#include "gtl/uuid.h"

#include "gcore/component.h"
#include "gcore/script/script_context.h"
#include "gcore/resource_handle.h"

namespace gcore
{
    struct script_component : component
    {
        struct script_info
        {
            gtl::uuid m_script_uuid;
            resource_handle<script> m_script;
            script_context m_context;
            void process(gserializer::serializer& serializer);
        };

        void process(gserializer::serializer& serializer) override;
        std::vector<script_info> m_scripts;

        GSERIALIZER_DECLARE_SUBCLASS_FACTORY_REGISTRATION();
    };
}