#include "stdafx.h"

#include "gserializer/serializers/binary_file_serializer.h"

#include "gtl/uuid.h"

namespace gserializer
{
    binary_file_reader::binary_file_reader(const char* file_path)
    {
        m_read_stream.open(file_path, std::ios::binary);
    }

    void binary_file_reader::process(const char*, std::string& value)
    {
        std::size_t size = 0;
        process("", size);
        std::unique_ptr<char[]> buffer = std::make_unique<char[]>(size + 1);
        m_read_stream.read(buffer.get(), size);
        buffer[size] = 0;
        value = buffer.get();
    }

    void binary_file_reader::process(const char*, std::uint32_t& value)
    {
        m_read_stream.read(reinterpret_cast<char*>(&value), sizeof(decltype(value)));
    }

    void binary_file_reader::process(const char*, std::uint64_t& value)
    {
        m_read_stream.read(reinterpret_cast<char*>(&value), sizeof(decltype(value)));
    }

    void binary_file_reader::process(const char*, std::int32_t& value)
    {
        m_read_stream.read(reinterpret_cast<char*>(&value), sizeof(decltype(value)));
    }

    void binary_file_reader::process(const char*, std::int64_t& value)
    {
        m_read_stream.read(reinterpret_cast<char*>(&value), sizeof(decltype(value)));
    }

    void binary_file_reader::process(const char*, float& value)
    {
        m_read_stream.read(reinterpret_cast<char*>(&value), sizeof(decltype(value)));
    }

    void binary_file_reader::process(const char*, double& value)
    {
        m_read_stream.read(reinterpret_cast<char*>(&value), sizeof(decltype(value)));
    }

    void binary_file_reader::process(const char*, bool& value)
    {
        m_read_stream.read(reinterpret_cast<char*>(&value), sizeof(decltype(value)));
    }

    void binary_file_reader::process(const char*, gtl::uuid& value)
    {
        m_read_stream.read(reinterpret_cast<char*>(&value), sizeof(decltype(value)));
    }

    void binary_file_reader::process(const char*, std::filesystem::path& value)
    {
        std::size_t size = 0;
        process("", size);
        std::unique_ptr<char[]> buffer = std::make_unique<char[]>(size + 1);
        m_read_stream.read(buffer.get(), size);
        buffer[size] = 0;
        value = buffer.get();
    }

    void binary_file_reader::open_array(const char*, std::size_t& element_count)
    {
        process("", element_count);
        m_array_element_stack.push_back(element_count);
    }

    void binary_file_reader::close_array(const char*)
    {
        m_array_element_stack.pop_back();
    }

    bool binary_file_reader::open_array_element(const char*)
    {
        return m_array_element_stack.back()-- != 0;
    }
    void binary_file_reader::close_array_element(const char*)
    {
    }

    binary_file_writer::binary_file_writer(const char* file_path)
    {
        m_write_stream.open(file_path, std::ios::binary);
    }
    
    void binary_file_writer::process(const char* name, std::string& value)
    {
        std::size_t size = value.size();
        process("", size);
        m_write_stream.write(value.data(), size);
    }
    
    void binary_file_writer::process(const char*, std::uint32_t& value)
    {
        m_write_stream.write(reinterpret_cast<char const*>(&value), sizeof(decltype(value)));
    }
    
    void binary_file_writer::process(const char*, std::uint64_t& value)
    {
        m_write_stream.write(reinterpret_cast<char const*>(&value), sizeof(decltype(value)));
    }
    
    void binary_file_writer::process(const char*, std::int32_t& value)
    {
        m_write_stream.write(reinterpret_cast<char const*>(&value), sizeof(decltype(value)));
    }
    
    void binary_file_writer::process(const char*, std::int64_t& value)
    {
        m_write_stream.write(reinterpret_cast<char const*>(&value), sizeof(decltype(value)));
    }
    
    void binary_file_writer::process(const char*, float& value)
    {
        m_write_stream.write(reinterpret_cast<char const*>(&value), sizeof(decltype(value)));
    }
    
    void binary_file_writer::process(const char*, double& value)
    {
        m_write_stream.write(reinterpret_cast<char const*>(&value), sizeof(decltype(value)));
    }
    
    void binary_file_writer::process(const char*, bool& value)
    {
        m_write_stream.write(reinterpret_cast<char const*>(&value), sizeof(decltype(value)));
    }
    
    void binary_file_writer::process(const char*, gtl::uuid& value)
    {
        m_write_stream.write(reinterpret_cast<char const*>(&value), sizeof(decltype(value)));
    }
    
    void binary_file_writer::process(const char*, std::filesystem::path& value)
    {
        std::size_t size = value.string().size();
        process("", size);
        m_write_stream.write(value.string().data(), size);
    }

    void binary_file_writer::open_array(const char* name, std::size_t& element_count)
    {
        process("",element_count);
    }
}


