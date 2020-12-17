#include "stdafx.h"

#include "gcore/resource_library.h"

#include "gserializer/serializer.h"
#include "gserializer/serializers/json_serializer.h"

namespace gcore
{
    void resource_library::process(gserializer::serializer& serializer)
    {
        serializer.process("uuid_to_resource_file", m_uuid_to_resource_file);
    }

    resource* resource_library::get_resource(gtl::uuid const& uuid)
    {
        {
            std::shared_lock lock(m_lock);
            if (auto const it = m_resource_map.find(uuid);
                it != m_resource_map.end())
            {
                return it->second.get();
            }
        }

        std::unique_lock lock(m_lock);
        auto const& [it, inserted] = m_resource_map.insert(std::pair(uuid, std::unique_ptr<resource>()));
        if (inserted)
        {
            if (auto const file_it = m_uuid_to_resource_file.find(uuid);
                file_it != m_uuid_to_resource_file.end())
            {
                gserializer::json_read_serializer serializer(file_it->second.c_str());
                serializer.process("resource", it->second, resource::factory());
                if (it->second)
                    it->second->load();
            }
        }
        return it->second.get();

    }
}
