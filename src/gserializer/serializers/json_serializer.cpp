#include "stdafx.h"
#include "gserializer/serializers/json_serializer.h"

#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/prettywriter.h>

#include <fstream>

gserializer::json_write_serializer::json_write_serializer()
{
    m_document.SetObject();
}

void gserializer::json_write_serializer::process(const char* name, std::string& value)
{
   stack_top().AddMember(rapidjson::Value(name, m_document.GetAllocator()), rapidjson::Value(value.c_str(), static_cast<rapidjson::SizeType>(value.size()), m_document.GetAllocator()), m_document.GetAllocator());
}

void gserializer::json_write_serializer::process(const char* name, std::uint32_t& value)
{
    stack_top().AddMember(rapidjson::Value(name, m_document.GetAllocator()), rapidjson::Value(value), m_document.GetAllocator());
}

void gserializer::json_write_serializer::process(const char* name, std::uint64_t& value)
{
    stack_top().AddMember(rapidjson::Value(name, m_document.GetAllocator()), rapidjson::Value(value), m_document.GetAllocator());
}

void gserializer::json_write_serializer::process(const char* name, std::int32_t& value)
{
    stack_top().AddMember(rapidjson::Value(name, m_document.GetAllocator()), rapidjson::Value(value), m_document.GetAllocator());
}

void gserializer::json_write_serializer::process(const char* name, std::int64_t& value)
{
    stack_top().AddMember(rapidjson::Value(name, m_document.GetAllocator()), rapidjson::Value(value), m_document.GetAllocator());
}

void gserializer::json_write_serializer::process(const char* name, float& value)
{
    stack_top().AddMember(rapidjson::Value(name, m_document.GetAllocator()), rapidjson::Value(value), m_document.GetAllocator());
}

void gserializer::json_write_serializer::process(const char* name, double& value)
{
    stack_top().AddMember(rapidjson::Value(name, m_document.GetAllocator()), rapidjson::Value(value), m_document.GetAllocator());
}

void gserializer::json_write_serializer::process(const char* name, bool& value)
{
    stack_top().AddMember(rapidjson::Value(name, m_document.GetAllocator()), rapidjson::Value(value), m_document.GetAllocator());
}

void gserializer::json_write_serializer::write_to_file(const char* file)
{
    std::ofstream ofs(file);
    rapidjson::OStreamWrapper osw(ofs);

    rapidjson::PrettyWriter<rapidjson::OStreamWrapper> writer(osw);
    m_document.Accept(writer);
}

rapidjson::Value& gserializer::json_write_serializer::stack_top()
{
    return m_stack.back().get();
}

void gserializer::json_write_serializer::open_scope(const char* name)
{
    create_and_add_stack_value(name, rapidjson::kObjectType);
}

void gserializer::json_write_serializer::close_scope(const char*)
{
    m_stack.pop_back();
}

void gserializer::json_write_serializer::open_array(const char* name, std::size_t& element_count)
{
    create_and_add_stack_value(name, rapidjson::kArrayType);
    stack_top().Reserve(static_cast<rapidjson::SizeType>(element_count), m_document.GetAllocator());
}

void gserializer::json_write_serializer::close_array(const char*)
{
    m_stack.pop_back();
}

bool gserializer::json_write_serializer::open_array_element()
{
    stack_top().PushBack(rapidjson::Value(rapidjson::kObjectType), m_document.GetAllocator());
    m_stack.push_back(*((stack_top().End() - 1)));
    return true;
}

void gserializer::json_write_serializer::close_array_element()
{
    m_stack.pop_back();
}

void gserializer::json_write_serializer::create_and_add_stack_value(const char* name, rapidjson::Type object_type)
{
    if (m_stack.size() == 0)
    {
        m_document.AddMember(rapidjson::Value(name, m_document.GetAllocator()), rapidjson::Value(object_type), m_document.GetAllocator());
        m_stack.push_back(m_document[name]);
    }
    else
    {
        stack_top().AddMember(rapidjson::Value(name, m_document.GetAllocator()), rapidjson::Value(object_type), m_document.GetAllocator());
        m_stack.push_back(stack_top()[name]);
    }
}

gserializer::json_read_serializer::json_read_serializer(const char* file_path)
{
    std::ifstream stream(file_path);
    rapidjson::IStreamWrapper isw(stream);
    m_document.ParseStream(isw);
}

void gserializer::json_read_serializer::process(const char* name, std::string& value)
{
    auto it = stack_top().FindMember(name);
    if (it != stack_top().MemberEnd())
    {
        value = it->value.GetString();
    }
}

void gserializer::json_read_serializer::process(const char* name, std::uint32_t& value)
{
    auto it = stack_top().FindMember(name);
    if (it != stack_top().MemberEnd())
    {
        value = it->value.GetUint();
    }
}

void gserializer::json_read_serializer::process(const char* name, std::uint64_t& value)
{
    auto it = stack_top().FindMember(name);
    if (it != stack_top().MemberEnd())
    {
        value = it->value.GetUint64();
    }
}

void gserializer::json_read_serializer::process(const char* name, std::int32_t& value)
{
    auto it = stack_top().FindMember(name);
    if (it != stack_top().MemberEnd())
    {
        value = it->value.GetInt();
    }
}

void gserializer::json_read_serializer::process(const char* name, std::int64_t& value)
{
    auto it = stack_top().FindMember(name);
    if (it != stack_top().MemberEnd())
    {
        value = it->value.GetInt64();
    }
}

void gserializer::json_read_serializer::process(const char* name, float& value)
{
    auto it = stack_top().FindMember(name);
    if (it != stack_top().MemberEnd())
    {
        value = it->value.GetFloat();
    }
}

void gserializer::json_read_serializer::process(const char* name, double& value)
{
    auto it = stack_top().FindMember(name);
    if (it != stack_top().MemberEnd())
    {
        value = it->value.GetDouble();
    }
}

void gserializer::json_read_serializer::process(const char* name, bool& value)
{
    auto it = stack_top().FindMember(name);
    if (it != stack_top().MemberEnd())
    {
        value = it->value.GetBool();
    }
}

rapidjson::Value& gserializer::json_read_serializer::stack_top()
{
    return m_stack.back().get();
}

void gserializer::json_read_serializer::open_scope(const char* name)
{
    create_and_add_stack_value(name, rapidjson::kObjectType);
}

void gserializer::json_read_serializer::close_scope(const char*)
{
    m_stack.pop_back();
}

void gserializer::json_read_serializer::open_array(const char* name, std::size_t& element_count)
{
    create_and_add_stack_value(name, rapidjson::kArrayType);
    m_array_itr_stack.push_back(stack_top().Begin());
    element_count = stack_top().Size();
}

void gserializer::json_read_serializer::close_array(const char* name)
{
    m_stack.pop_back();
    m_array_itr_stack.pop_back();
}

bool gserializer::json_read_serializer::open_array_element()
{
    if (m_array_itr_stack.back() == stack_top().End())
    {
        return false;
    }

    m_stack.push_back(*m_array_itr_stack.back());
    m_array_itr_stack.back()++;
    return true;
}

void gserializer::json_read_serializer::close_array_element()
{
    m_stack.pop_back();
}

void gserializer::json_read_serializer::create_and_add_stack_value(const char* name, rapidjson::Type object_type)
{
    if (m_stack.size() == 0)
    {
        if (!m_document.IsObject())
            m_document.SetObject();
        auto it = m_document.FindMember(name);
        if (it == m_document.MemberEnd())
            m_document.AddMember(rapidjson::Value(name, m_document.GetAllocator()), rapidjson::Value(object_type), m_document.GetAllocator());
        m_stack.push_back(m_document[name]);
    }
    else
    {
        auto it = m_stack.back().get().FindMember(name);
        if (it == m_stack.back().get().MemberEnd())
            m_stack.back().get().AddMember(rapidjson::Value(name, m_document.GetAllocator()), rapidjson::Value(object_type), m_document.GetAllocator());
        m_stack.push_back(m_stack.back().get()[name]);
    }
}


