#include "stdafx.h"
#include "uuid.h"

#include <chrono>
#include <atomic>
#include <random>
#include <charconv>

namespace gtl
{
    uuid uuid::generate()
    {
        static thread_local std::random_device                              rd;
        static thread_local std::mt19937                                    gen(rd());
        static thread_local std::uniform_int_distribution<std::uint64_t>    dis(0);
        static std::atomic<uint32_t> counter = static_cast<uint32_t>(dis(gen));
        
        auto const time_since_epoch = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        std::uint64_t const high = static_cast<std::uint64_t>(time_since_epoch) << 32ull | counter++;
        std::uint64_t const low = dis(gen);
        return uuid(high, low);
    }

    uuid uuid::from_string(std::string_view string)
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
    std::string uuid::to_string() const
    {
        constexpr std::size_t buffer_size = ((CHAR_BIT * sizeof(std::uint64_t) * 2) >> 2) + 1;
        char buffer[buffer_size];
        std::to_chars_result const results_high = std::to_chars(std::begin(buffer), std::end(buffer), m_array[0], 16);
        std::to_chars_result const results_low = std::to_chars(results_high.ptr, std::end(buffer), m_array[1], 16);
        (*results_low.ptr) = 0;
        return std::string(buffer);
    }
}

