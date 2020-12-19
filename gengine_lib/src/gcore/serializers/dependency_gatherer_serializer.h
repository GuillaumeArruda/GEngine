#pragma once

#include "gserializer/serializer.h"

#include "gtl/uuid.h"

namespace gcore
{
    struct dependency_gatherer_serializer : gserializer::serializer
    {
        using serializer::process;
        virtual bool is_reading_from_object() const { return true; };

        virtual void process(const char* name, std::string& value) {};
        virtual void process(const char* name, std::uint32_t& value) {};
        virtual void process(const char* name, std::uint64_t& value) {};
        virtual void process(const char* name, std::int32_t& value) {};
        virtual void process(const char* name, std::int64_t& value) {};
        virtual void process(const char* name, float& value) {};
        virtual void process(const char* name, double& value) {};
        virtual void process(const char* name, bool& value) {};
        virtual void process(const char* name, gtl::uuid& value);
        virtual void process(const char* name, std::filesystem::path& value);

        std::vector<gtl::uuid> m_uuids;
        std::vector<std::filesystem::path> m_files;

    private:
        virtual void open_scope(const char* name) {};
        virtual void close_scope(const char* name) {};
        virtual bool enum_as_string() const { return false; };

        virtual void open_array(const char* name, std::size_t& element_count) {};
        virtual void close_array(const char* name) {};
        virtual bool open_array_element(const char* name) { return true; }
        virtual void close_array_element(const char* name) {};
    };
}