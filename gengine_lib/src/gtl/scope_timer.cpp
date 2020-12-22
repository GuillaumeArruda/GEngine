#include "stdafx.h"

#include "scope_timer.h"

#include <iostream>

namespace gtl
{
    scope_timer::scope_timer(const char* name) : m_name(name)
    {
        m_start = std::chrono::high_resolution_clock::now();
    }

    scope_timer::~scope_timer()
    {
        std::chrono::time_point<std::chrono::high_resolution_clock> const end = std::chrono::high_resolution_clock::now();
        std::cout << m_name << " time: " << std::chrono::duration_cast<std::chrono::microseconds>(end - m_start).count() << " us\n";
    }
}

