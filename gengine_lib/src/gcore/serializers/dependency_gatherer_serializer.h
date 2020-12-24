#pragma once

#include "gserializer/serializer.h"

#include "gtl/uuid.h"

namespace gcore
{
    struct dependency_gatherer_serializer : gserializer::serializer
    {
        using serializer::process;
        virtual bool is_reading_from_object() const { return true; };

        virtual void process(const char* , std::string& ) {};
        virtual void process(const char* , std::uint32_t& ) {};
        virtual void process(const char* , std::uint64_t& ) {};
        virtual void process(const char* , std::int32_t& ) {};
        virtual void process(const char* , std::int64_t& ) {};
        virtual void process(const char* , float& ) {};
        virtual void process(const char* , double& ) {};
        virtual void process(const char* , bool& ) {};
        virtual void process(const char* name, gtl::uuid& value);
        virtual void process(const char* name, std::filesystem::path& value);

        std::vector<gtl::uuid> m_uuids;
        std::vector<std::filesystem::path> m_files;

    private:
        virtual void open_scope(const char*) {};
        virtual void close_scope(const char*) {};
        virtual bool enum_as_string() const { return false; };

        virtual void open_array(const char*, std::size_t&) {};
        virtual void close_array(const char*) {};
        virtual bool open_array_element(const char*) { return true; }
        virtual void close_array_element(const char*) {};
    };
}