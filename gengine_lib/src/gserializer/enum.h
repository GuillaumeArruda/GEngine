#pragma once

#include <algorithm>
#include <array>

#define GSERIALIZER_BEGIN_ENUM_HANDLER(Enum)                                                                \
struct string_pair_##Enum {                                                                                 \
    constexpr string_pair_##Enum(Enum value, const char* string) : m_value(value), m_string(string) {}      \
    Enum m_value;                                                                                           \
    const char* m_string;                                                                                   \
};                                                                                                          \
static constexpr string_pair_##Enum to_string_array_##Enum[] = {    

#define GSERIALIZER_ADD_ENUM_ELEMENT(EnumElement)                                                           \
    { EnumElement, #EnumElement },

#define GSERIALIZER_END_ENUM_HANDLER(Enum)                                                                  \
    };                                                                                                      \
constexpr const char* to_string(Enum value)                                                                 \
{                                                                                                           \
    auto it = std::find_if(std::begin(to_string_array_##Enum), std::end(to_string_array_##Enum),            \
        [&](auto& pair) { return pair.m_value == value; });                                                 \
    if(it != std::end(to_string_array_##Enum))                                                              \
        return it->m_string;                                                                                \
    return #Enum "::<Unkwnon>";                                                                             \
}                                                                                                           \
                                                                                                            \
constexpr bool from_string(const char* string, Enum& value)                                                 \
{                                                                                                           \
    auto it = std::find_if(std::begin(to_string_array_##Enum), std::end(to_string_array_##Enum),            \
        [&](auto& pair) { return std::strcmp(pair.m_string, string) == 0; });                               \
        if(it != std::end(to_string_array_##Enum))                                                          \
        {                                                                                                   \
            value = it->m_value;                                                                            \
            return true;                                                                                    \
        }                                                                                                   \
        return false;                                                                                       \
}