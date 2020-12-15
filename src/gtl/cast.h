#pragma once
#include <utility>
#include <type_traits>

#define GTL_CHECK_CAST() 0

namespace gtl
{
    namespace details
    {
        template<class To, class From>
        void check_valid_cast(From&& from)
        {
#if GTL_CHECK_CAST()
            if constexpr (std::is_polymorphic_v<std::remove_pointer_t<std::decay_t<To>>> && std::is_polymorphic_v<std::remove_pointer_t<std::decay_t<From>>>)
            {
                if constexpr (std::is_pointer_v<std::decay_t<From>>)
                {
                    if (from != nullptr)
                        dynamic_cast<std::add_lvalue_reference_t<std::remove_pointer_t<To>>>(*std::forward<From>(from));
                }
                else
                {
                    dynamic_cast<To>(std::forward<From>(from));
                }
            }
#endif
        }
    }
    template<class To, class From>
    auto cast(From&& from)
    {
        details::check_valid_cast<To>(std::forward<From>(from));
        return static_cast<To>(std::forward<From>(from));
    }
}