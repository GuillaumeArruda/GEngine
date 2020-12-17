#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <compare>

namespace gtl
{
    struct uuid
    {
        static uuid generate();
        static uuid from_string(std::string_view string);

       constexpr uuid(std::uint64_t high = 0, std::uint64_t low = 0) : m_array{ high, low} {}
        
        constexpr bool is_valid() const { return m_array[0] != 0 && m_array[1] != 0; }
        std::string to_string() const;

        constexpr auto operator<=>(uuid const&) const = default;
    private:
        friend std::hash<uuid>;
        std::uint64_t m_array[2] = { 0ull, 0ull };
    };
}

namespace std
{
    template<>
    struct hash<gtl::uuid>
    {
        using argument_type = gtl::uuid;
        using result_type = std::size_t;
        std::size_t operator()(gtl::uuid const& key) const noexcept
        {
            return key.m_array[1]; // low bit are already random no need to hash I think
        }
    };
}

inline gtl::uuid operator"" _gtl_uuid(const char* string, std::size_t length)
{
    return gtl::uuid::from_string({ string, length });
}