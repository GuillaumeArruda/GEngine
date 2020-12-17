#pragma once

#include "gtl/uuid.h"
#include "gserializer/type_factory.h"

namespace gserializer
{
    struct serializer;
}

namespace gcore
{
    struct resource
    {
        resource(gtl::uuid const& uuid) : m_uuid(uuid) {}
        virtual ~resource() = 0;

        virtual void process(gserializer::serializer& serializer);
        virtual void load() = 0;
        
        gtl::uuid const& get_uuid() const noexcept{ return m_uuid; }

        GSERIALIZER_DECLARE_FACTORY_BASE(resource);
    private:
        gtl::uuid m_uuid;
    };
}