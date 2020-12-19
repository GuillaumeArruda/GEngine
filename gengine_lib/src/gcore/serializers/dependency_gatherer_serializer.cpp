#include "stdafx.h"

#include "gcore/serializers/dependency_gatherer_serializer.h"

namespace gcore
{
    void dependency_gatherer_serializer::process(const char* name, gtl::uuid& value)
    {
        m_uuids.push_back(value);
    }
    void dependency_gatherer_serializer::process(const char* name, std::filesystem::path& value)
    {
        m_files.push_back(value);
    }
}


