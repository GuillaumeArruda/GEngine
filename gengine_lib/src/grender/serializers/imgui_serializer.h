#pragma once

#include "gserializer/serializer.h"

namespace grender
{
    struct imgui_serializer : gserializer::serializer
    {
        using serializer::process;
        imgui_serializer(const char* node_name);
        ~imgui_serializer();

        virtual bool is_reading_from_object() const { return true; }
        virtual bool is_writing_to_object() const { return true; }

        void process(const char* name, std::string& value) override;
        void process(const char* name, std::uint32_t& value) override;
        void process(const char* name, std::uint64_t& value) override;
        void process(const char* name, std::int32_t& value) override;
        void process(const char* name, std::int64_t& value) override;
        void process(const char* name, float& value) override;
        void process(const char* name, double& value) override;
        void process(const char* name, bool& value) override;
        void process(const char* name, gtl::uuid& value) override;

    private:
        bool should_display() const { return m_should_display_stack.back(); }
        std::vector<bool> m_should_display_stack;
        std::vector<std::size_t> m_array_index_stack;
        std::vector<std::size_t> m_array_element_stack;
        bool enum_as_string() const override { return true; }

        void open_scope(const char* name) override;
        void close_scope(const char* name) override;

        void open_array(const char* name, std::size_t& element_count) override;
        void close_array(const char* name) override;
        bool open_array_element() override;
        void close_array_element() override;
    };
}