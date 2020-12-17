#pragma once

#include <unordered_map>
#include <string>
#include <memory>
#include <shared_mutex>

#include "gtl/uuid.h"

#include "gcore/resource.h"

namespace gserializer
{
    struct serializer;
}

namespace gcore
{
    struct resource_library
    {
        void process(gserializer::serializer& serializer);

        template<class ResourceType>
        ResourceType* get_resource(gtl::uuid const& uuid)
        {
            return dynamic_cast<ResourceType*>(get_resource(uuid));
        }

    private:
        resource* get_resource(gtl::uuid const& uuid);

        std::unordered_map<gtl::uuid, std::string> m_uuid_to_resource_file;
        std::unordered_map<gtl::uuid, std::unique_ptr<resource>> m_resource_map;
        std::shared_mutex m_lock;
    };
}