#pragma once

#include <chrono>

namespace gtl
{
    struct scope_timer
    {
        scope_timer(const char* name);
        ~scope_timer();
        const char* m_name = nullptr;
        std::chrono::time_point<std::chrono::high_resolution_clock> m_start;
    };
}