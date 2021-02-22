#pragma once

#include "gserializer/serializer.h"

#include "gtl/uuid.h"

namespace gcore
{
    struct dependency_gatherer_serializer : gserializer::serializer
    {
        dependency_gatherer_serializer(bool can_write_to_object = false) : m_can_write_to_object(can_write_to_object) {}
        using serializer::process;
        bool is_reading_from_object() const override { return true; };
        bool is_writing_to_object() const override { return m_can_write_to_object; }

        void process(const char* , std::string& ) override {};
        void process(const char* , std::uint32_t& ) override {};
        void process(const char* , std::uint64_t& ) override {};
        void process(const char* , std::int32_t& ) override {};
        void process(const char* , std::int64_t& ) override {};
        void process(const char* , float& ) override {};
        void process(const char* , double& ) override {};
        void process(const char* , bool& ) override {};
        void process(const char* name, gtl::uuid& value) override;
        void process(const char* name, std::filesystem::path& value) override;

        std::vector<gtl::uuid> m_uuids;
        std::vector<std::filesystem::path> m_files;

    private:
        bool m_can_write_to_object;
        struct array_information
        {
            std::size_t m_location = 0;
            std::size_t m_size = 0;
        };
        std::vector<array_information> m_array_size_stack;
        void open_scope(const char*) override {};
        void close_scope(const char*) override {};
        bool enum_as_string() const override { return false; };

        void open_array(const char*, std::size_t&) override;
        void close_array(const char*) override;
        bool open_array_element(const char*) override;
        void close_array_element(const char*) override;;
    };
}