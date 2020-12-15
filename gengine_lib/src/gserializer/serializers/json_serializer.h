#pragma once

#include "gserializer/serializer.h"

#include <rapidjson/document.h>

namespace gserializer
{
    struct json_write_serializer : serializer
    {    
        json_write_serializer();

        using serializer::process;
        bool is_writing() const { return true; }
        void process(const char* name, std::string& value) override;
        void process(const char* name, std::uint32_t& value) override;
        void process(const char* name, std::uint64_t& value) override;
        void process(const char* name, std::int32_t& value) override;
        void process(const char* name, std::int64_t& value) override;
        void process(const char* name, float& value) override;
        void process(const char* name, double& value) override;
        void process(const char* name, bool& value) override;

        void write_to_file(const char* file);
    private:
        rapidjson::Value& stack_top();

        void open_scope(const char* name) override;
        void close_scope(const char* name) override;
        bool enum_as_string() const override { return true; }

        void open_array(const char* name, std::size_t& element_count) override;
        void close_array(const char* name) override;
        bool open_array_element() override;
        void close_array_element() override;

        void create_and_add_stack_value(const char* name, rapidjson::Type object_type);

        rapidjson::Document m_document;
        std::vector<std::reference_wrapper<rapidjson::Value>> m_stack;
    };

    struct json_read_serializer : serializer
    {
        json_read_serializer(const char* file_path);

        using serializer::process;
        bool is_writing() const { return false; }
        void process(const char* name, std::string& value) override;
        void process(const char* name, std::uint32_t& value) override;
        void process(const char* name, std::uint64_t& value) override;
        void process(const char* name, std::int32_t& value) override;
        void process(const char* name, std::int64_t& value) override;
        void process(const char* name, float& value) override;
        void process(const char* name, double& value) override;
        void process(const char* name, bool& value) override;

    private:
        rapidjson::Value& stack_top();

        void open_scope(const char* name) override;
        void close_scope(const char* name) override;
        bool enum_as_string() const override { return true; }

        void open_array(const char* name, std::size_t& element_count) override;
        void close_array(const char* name) override;
        bool open_array_element() override;
        void close_array_element() override;

        void create_and_add_stack_value(const char* name, rapidjson::Type object_type);

        rapidjson::Document m_document;
        std::vector<std::reference_wrapper<rapidjson::Value>> m_stack;
        std::vector<rapidjson::Value*> m_array_itr_stack;
        std::vector<rapidjson::Value::MemberIterator> m_map_itr_stack;
    };
}