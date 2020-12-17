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
        virtual ~resource() {};

        virtual void process(gserializer::serializer& serializer);
        virtual void load() = 0;
        
        gtl::uuid const& get_uuid() const noexcept{ return m_uuid; }

        GSERIALIZER_DECLARE_FACTORY_BASE(resource);
    private:
        gtl::uuid m_uuid = gtl::uuid::generate();
        std::string m_name;
    };
}