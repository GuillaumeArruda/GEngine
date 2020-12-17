#pragma once

#include <vector>

#include "gcore/system.h"

struct GLFWwindow;

namespace gcore
{
    struct window_system : system
    {
        window_system(std::string const& window_name, int x_resolution, int y_resolution);
        ~window_system() override;
        window_system(window_system&&) noexcept = default;
        window_system& operator=(window_system&&);

        window_system(window_system const&) = delete;
        window_system& operator=(window_system const&) = delete;

        void bind_context();
        bool should_close() const;

    private:
        GLFWwindow* m_window;
    };
}