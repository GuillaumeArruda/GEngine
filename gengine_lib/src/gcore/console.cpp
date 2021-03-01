#include "stdafx.h"

#include <charconv>

#include "gcore/console.h"

#include "gserializer/serializer.h"

namespace gcore
{
    namespace details
    {
        bool from_string(std::string_view const& string_value, int* value)
        {
            auto result = std::from_chars(string_value.data(), string_value.data() + string_value.size(), *value);
            return result.ec == std::errc();
        }

        bool from_string(std::string_view const& string_value, float* value)
        {
            auto result = std::from_chars(string_value.data(), string_value.data() + string_value.size(), *value);
            return result.ec == std::errc();
        }

        bool from_string(std::string_view const& string_value, std::string* value)
        {
            *value = string_value;
            return true;
        }

        bool from_string(std::string_view const& string_value, bool* value)
        {
            int int_value = 0;
            std::from_chars_result const result = std::from_chars(string_value.data(), string_value.data() + string_value.size(), int_value);
            if (result.ec == std::errc())
            {
                *value = static_cast<bool>(int_value);
                return true;
            }
            return false;
        }

        bool from_string(std::string_view const& string_value, std::function<bool(std::string_view)>& callback)
        {
            return callback(string_value);
        }
    }

    console& console::get()
    {
        static console instance;
        return instance;
    }

    void console::register_value(std::string_view name, value value)
    {
        m_values[name] = std::move(value);
        m_has_been_modified = true;
    }

    void console::unregister_value(std::string_view name)
    {
        m_values.erase(name);
        m_has_been_modified = true;
    }

    bool console::execute(std::string_view console_view)
    {
        std::size_t const position = console_view.find_first_of(' ');
        std::string_view const console_command = position == std::string_view::npos ? console_view : console_view.substr(0, position);
        std::string_view const console_value = position == std::string_view::npos ? std::string_view() : console_view.substr(position + 1);

        if (auto it = m_values.find(console_command);
            it != m_values.end())
        {
            return std::visit([&](auto& value) { return details::from_string(console_value, value); }, it->second);;
        }
        return false;
    }
    std::vector<std::string_view> console::get_possible_commands() const
    {
        std::vector<std::string_view> commands;
        for (auto& [name, value] : m_values)
            commands.push_back(name);
        return commands;
    }

    console::value const* console::get_value(std::string_view name) const
    {
        if (auto const it = m_values.find(name); 
            it != m_values.end())
        {
            return &it->second;
        }
        return nullptr;
    }
    bool console::consume_has_been_modified()
    {
        bool const has_been_modified = m_has_been_modified;
        m_has_been_modified = false;
        return has_been_modified;
    }
}


