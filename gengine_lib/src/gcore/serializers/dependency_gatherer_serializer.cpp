#include "stdafx.h"

#include "gcore/serializers/dependency_gatherer_serializer.h"

namespace gcore
{
    void dependency_gatherer_serializer::process(const char*, gtl::uuid& value)
    {
        m_uuids.push_back(value);
    }
    void dependency_gatherer_serializer::process(const char*, std::filesystem::path& value)
    {
        m_files.push_back(value);
    }

    void dependency_gatherer_serializer::open_array(const char*, std::size_t& size) 
    {
        if (m_can_write_to_object)
        {
            m_array_size_stack.push_back(array_information{ 0, size });
        }
    }
    void dependency_gatherer_serializer::close_array(const char*) {
        if (m_can_write_to_object)
        {
            m_array_size_stack.pop_back();
        }
    }
    bool dependency_gatherer_serializer::open_array_element(const char*) 
    { 
        if (m_can_write_to_object)
        {
            return (m_array_size_stack.back().m_location++) < m_array_size_stack.back().m_size;
        }
        return true;
    }
    void dependency_gatherer_serializer::close_array_element(const char*) {}
}


