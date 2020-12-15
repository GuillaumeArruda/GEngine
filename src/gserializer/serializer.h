#pragma once

#include <string>
#include <memory>
#include <vector>
#include <optional>

namespace gserializer
{
    struct serializer;
    template<class TypeToSerialize>
    auto process(serializer& serializer, TypeToSerialize& value) -> decltype(value.process(serializer))
    {
        value.process(serializer);
    }

    struct serializer
    {
        struct scope
        {
            scope(const char* name, serializer& serializer) : m_name(name), m_serializer(serializer) { serializer.open_scope(m_name); };
            ~scope() { m_serializer.close_scope(m_name); }
            const char* m_name;
            serializer& m_serializer;
        };

        virtual bool is_writing() const = 0;
        virtual bool is_reading() const { return !is_writing(); }

        virtual void process(const char* name, std::string& value) = 0;
        virtual void process(const char* name, std::uint32_t& value) = 0;
        virtual void process(const char* name, std::uint64_t& value) = 0;
        virtual void process(const char* name, std::int32_t& value) = 0;
        virtual void process(const char* name, std::int64_t& value) = 0;
        virtual void process(const char* name, float& value) = 0;
        virtual void process(const char* name, double& value) = 0;
        virtual void process(const char* name, bool& value) = 0;


        template<class TypeToSerialize, class ... ExtraType>
        void process(const char* name, TypeToSerialize& value, ExtraType&& ... extras)
        {
            using gserializer::process;
            scope s(name, *this);
            process(*this, value, std::forward<ExtraType>(extras)...);
        }

        template<class TypeToSerialize, class ...ExtraType >
        void process(const char* name, std::vector<TypeToSerialize>& container, ExtraType&&... extras)
        {
            using gserializer::process;
            std::size_t element_count = container.size();
            open_array(name, element_count);
            if (is_writing())
            {
                for (auto& value : container)
                {
                    open_array_element();
                    process(*this, value, std::forward<ExtraType>(extras)...);
                    close_array_element();
                }
            }
            else
            {
                container.reserve(element_count);
                while (open_array_element())
                {
                    TypeToSerialize& element = container.emplace_back();
                    process(*this, element , std::forward<ExtraType>(extras)...);
                    close_array_element();
                }
            }
            close_array(name);
        }

        template<class Key, class TypeToSerialize, class ...ExtraType>
        void process(const char* name, std::unordered_map<Key, TypeToSerialize>& container, ExtraType&&... extras)
        {
            using gserializer::process;
            std::size_t element_count = container.size();
            open_array(name, element_count);
            if (is_writing())
            {
                for (auto& value : container)
                {
                    open_array_element();
                    Key key = value.first;
                    this->process("key", key);
                    this->process("data", value.second, std::forward<ExtraType>(extras)...);
                    close_array_element();
                }
            }
            else
            {
                container.reserve(element_count);
                while (open_array_element())
                {
                    Key key;
                    this->process("key", key);
                    this->process("data", container[key], std::forward<ExtraType>(extras)...);
                    close_array_element();
                }
            }
            close_array(name);
        }


        template<class EnumToSerialize>
        auto process(const char* name, EnumToSerialize& value) -> std::enable_if_t<std::is_enum_v<EnumToSerialize>>
        {
            if (enum_as_string())
            {
                std::string temp;
                if (is_reading())
                {
                    process(name, temp);
                    from_string(temp.c_str(), value);
                }
                else
                {
                    temp = to_string(value);
                    process(name, temp);
                }
            }
            else
            {
                std::underlying_type_t<EnumToSerialize> int_value = static_cast<std::underlying_type_t<EnumToSerialize>>(value);
                process(name, int_value);
                if (is_reading())
                    value = static_cast<EnumToSerialize>(int_value);
            }
        }

    private:
        virtual void open_scope(const char* name) = 0;
        virtual void close_scope(const char* name) = 0;
        
        virtual bool enum_as_string() const = 0;

        virtual void open_array(const char* name, std::size_t& element_count) = 0;
        virtual void close_array(const char* name) = 0;
        virtual bool open_array_element() = 0;
        virtual void close_array_element() = 0;
    };

    template<typename TypeToSerialize>
    void process(serializer& serializer, std::optional<TypeToSerialize>& value)
    {
        bool is_valid = static_cast<bool>(value);
        serializer.process("is_valid", is_valid);
        if (serializer.is_writing())
        {
            if (is_valid)
            {
                serializer.process("data", *value);
            }
        }
        else
        {
            if (is_valid)
            {
                value = TypeToSerialize();
                serializer.process("data", *value);
            }
        }
    }

    template<class TypeToSerialize>
    void process(serializer& serializer, std::unique_ptr<TypeToSerialize>& value)
    {
        bool is_valid = static_cast<bool>(value);
        serializer.process("is_valid", is_valid);
        if (serializer.is_writing())
        {
            if (value)
                serializer.process("data", *value);
        }
        else
        {
            if (is_valid)
            {
                value = std::make_unique<TypeToSerialize>();
                serializer.process("data", *value);
            }
        }
    }

    template<class TypeToSerialize, class Factory>
    void process(serializer& serializer, std::unique_ptr<TypeToSerialize>& value, Factory const& factory)
    {
        bool is_valid = static_cast<bool>(value);
        serializer.process("is_valid", is_valid);
        if (is_valid)
        {
            if (serializer.is_writing())
            {
                std::string type_name(factory.get_type_name(*value));
                serializer.process("type_name", type_name);
                serializer.process("data", *value);

            }
            else
            {
                std::string type_name;
                serializer.process("type_name", type_name);
                value = factory.create(type_name);
                if (value)
                {
                    serializer.process("data", *value);
                }
            }
        }
    }

    template<class TypeToSerialize>
    void process(serializer& serializer, std::shared_ptr<TypeToSerialize>& value)
    {
        bool is_valid = static_cast<bool>(value);
        serializer.process("is_valid", is_valid);
        if (serializer.is_writing())
        {
            if (value)
                serializer.process("data", *value);
        }
        else
        {
            if (is_valid)
            {
                value = std::make_shared<TypeToSerialize>();
                serializer.process("data", *value);
            }
        }
    }

    template<class TypeToSerialize, class Factory>
    void process(serializer& serializer, std::shared_ptr<TypeToSerialize>& value, Factory const& factory)
    {
        bool is_valid = static_cast<bool>(value);
        serializer.process("is_valid", is_valid);
        if (is_valid)
        {
            if (serializer.is_writing())
            {
                std::string type_name(factory.get_type_name(*value));
                serializer.process("type_name", type_name);
                serializer.process("data", *value);

            }
            else
            {
                std::string type_name;
                serializer.process("type_name", type_name);
                value = factory.create(type_name);
                if (value)
                    serializer.process("data", *value);
            }
        }
    }
}

