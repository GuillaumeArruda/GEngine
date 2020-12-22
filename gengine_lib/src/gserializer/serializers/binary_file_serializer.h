#pragma once

#include "gserializer/serializer.h"

#include <fstream>

namespace gserializer
{
    struct binary_file_reader : serializer
    {
        using serializer::process;
        binary_file_reader(const char* file_path);
        bool is_reading_from_object() const override { return false; }
        bool enum_as_string() const { return false; };
        
        
        void process(const char* name, std::string& value) override;
        void process(const char* name, std::uint32_t& value) override;
        void process(const char* name, std::uint64_t& value) override;
        void process(const char* name, std::int32_t& value) override;
        void process(const char* name, std::int64_t& value) override;
        void process(const char* name, float& value) override;
        void process(const char* name, double& value) override;
        void process(const char* name, bool& value) override;
        void process(const char* name, gtl::uuid& value) override;
        void process(const char* name, std::filesystem::path& value) override;
        
        void open_scope(const char* name) override {};
        void close_scope(const char* name) override {};
        void open_array(const char* name, std::size_t& element_count) override;
        void close_array(const char* name) override;
        bool open_array_element(const char* name) override;
        void close_array_element(const char* name) override;

        std::vector<std::size_t> m_array_element_stack;
        std::ifstream m_read_stream;
    };

    struct binary_file_writer : serializer
    {
        using serializer::process;
        binary_file_writer(const char* file_path);
        bool is_reading_from_object() const override { return true; }
        bool enum_as_string() const { return false; };

        void process(const char* name, std::string& value) override;
        void process(const char* name, std::uint32_t& value) override;
        void process(const char* name, std::uint64_t& value) override;
        void process(const char* name, std::int32_t& value) override;
        void process(const char* name, std::int64_t& value) override;
        void process(const char* name, float& value) override;
        void process(const char* name, double& value) override;
        void process(const char* name, bool& value) override;
        void process(const char* name, gtl::uuid& value) override;
        void process(const char* name, std::filesystem::path& value) override;

        void open_scope(const char* name) override {};
        void close_scope(const char* name) override {};
        void open_array(const char* name, std::size_t& element_count) override;
        void close_array(const char* name) override {}
        bool open_array_element(const char* name) override { return true; }
        void close_array_element(const char* name) override {}

        std::ofstream m_write_stream;
    };
}