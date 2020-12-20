#pragma once

#include "gtool/window.h"

namespace gtool
{
    struct viewport_window : window
    {
        void update(gcore::world& world, window_manager& manager) override;
        bool& get_should_display() override { return m_display; }
        const char* get_name() const override { return "Viewport"; }
    private:
        bool m_display = true;
    };
}
