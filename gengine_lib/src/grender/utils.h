#pragma once

#include <iostream>

#include <GL/glew.h>

#define GLCHECKERROR 1
namespace grender
{
#if GLCHECKERROR
    void check_gl_error();

    template<class Function, class ... Args>
    auto gl_exec(Function function, Args&& ... args)
    {
        if constexpr (std::is_same_v<std::invoke_result_t<Function, Args...>, void>)
        {
            function(std::forward<Args>(args)...);
            check_gl_error();
        }
        else
        {
            auto value = function(std::forward<Args>(args)...);
            check_gl_error();
            return value;
        }
    }   
#else
    #define gl_exec(Function, ...) Function(__VA_ARGS__);
#endif
}