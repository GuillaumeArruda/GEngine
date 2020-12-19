#pragma once

#include <type_traits>
#include "gtl/cast.h"

namespace gtl
{
    template<class Enum>
    constexpr auto to_underlying(Enum e) ->std::enable_if_t<std::is_enum_v<Enum>, std::underlying_type_t<Enum>>
    {
        return static_cast<std::underlying_type_t<Enum>>(e);
    }
}