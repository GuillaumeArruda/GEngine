#pragma once

#include <algorithm>
#include <array>

#define GSERIALIZER_BEGIN_ENUM_HANDLER(Enum)                                                                \
namespace string_pair##Enum                                                                                 \
{                                                                                                           \
    struct string_pair {                                                                                    \
    constexpr string_pair(Enum value, const char* string) : m_value(value), m_string(string) {}             \
    Enum m_value;                                                                                           \
    const char* m_string;                                                                                   \
    };                                                                                                      \
    static constexpr string_pair to_string_array[] = {                                                      




#define GSERIALIZER_ADD_ENUM_ELEMENT(EnumElement)                                                           \
    { EnumElement, #EnumElement },

#define GSERIALIZER_END_ENUM_HANDLER(Enum)                                                                  \
    };                                                                                                      \
}                                                                                                           \
constexpr const char* to_string(Enum value)                                                                 \
{                                                                                                           \
    auto it = std::find_if(std::begin(string_pair##Enum::to_string_array),                                  \
                           std::end(string_pair##Enum::to_string_array),                                    \
        [&](auto& pair) { return pair.m_value == value; });                                                 \
    if(it != std::end(string_pair##Enum::to_string_array))                                                  \
        return it->m_string;                                                                                \
    return #Enum "::<Unkwnon>";                                                                             \
}                                                                                                           \
                                                                                                            \
constexpr bool from_string(const char* string, Enum& value)                                                 \
{                                                                                                           \
    auto it = std::find_if(std::begin(string_pair##Enum::to_string_array),                                  \
                           std::end(string_pair##Enum::to_string_array),                                    \
        [&](auto& pair) { return std::strcmp(pair.m_string, string) == 0; });                               \
        if(it != std::end(string_pair##Enum::to_string_array))                                              \
        {                                                                                                   \
            value = it->m_value;                                                                            \
            return true;                                                                                    \
        }                                                                                                   \
        return false;                                                                                       \
}

#define GSERIALIZER_FORWARD_DECLARE_ENUM_SERIALIZATION(Enum)                                                \
    constexpr const char* to_string(Enum);                                                                  \
    constexpr bool from_string(const char* string, Enum& value);