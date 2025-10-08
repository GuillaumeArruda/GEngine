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
        static constexpr uuid from_string(std::string_view string);

       constexpr uuid(std::uint64_t high = 0, std::uint64_t low = 0) : m_array{ high, low} {}
        
        constexpr bool is_valid() const { return m_array[0] != 0 && m_array[1] != 0; }
        std::string to_string() const;

        constexpr auto operator<=>(uuid const&) const = default;
    private:
        friend std::hash<uuid>;
        std::uint64_t m_array[2] = { 0ull, 0ull };
    };

    constexpr uuid uuid::from_string(std::string_view string)
    {
        std::uint64_t high = 0;
        std::uint64_t low = 0;
        if (auto const from_char_result_high = std::from_chars(string.data(), string.data() + std::min(string.size(), (CHAR_BIT * sizeof(std::uint64_t) >> 2)), high, 16);
            from_char_result_high.ec == std::errc())
        {
            if (auto const from_char_result_low = std::from_chars(from_char_result_high.ptr, string.data() + string.size(), low, 16);
                from_char_result_low.ec == std::errc())

            {
                return uuid(high, low);
            }
        }
        return uuid();
    }
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

inline consteval gtl::uuid operator"" _gtl_uuid(const char* string, std::size_t length)
{
    return gtl::uuid::from_string({ string, length });
}