#pragma once

#include <memory>
#include <vector>

#include "gtool/window.h"

namespace gcore
{
    struct world;
}

struct GLFWwindow;

namespace gtool
{
    struct window_manager
    {
        window_manager();
        ~window_manager();
        
        void update(gcore::world& world);

        bool wants_to_close() const;

        GLFWwindow* get_window() { return m_window; }
    private:
        GLFWwindow* m_window;
        std::vector<std::unique_ptr<window>> m_windows;
    };
}