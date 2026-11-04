#pragma once

#include "gtool/window.h"

namespace gtool
{
    struct viewport_window : window
    {
        viewport_window(std::string name) : m_name(name) {}
        void update(gcore::world& world, window_manager& manager) override;
        bool& get_should_display() override { return m_display; }
        const char* get_name() const override { return m_name.c_str(); }
        float get_aspect_ratio() const { return m_size.x / static_cast<float>(m_size.y); }
    private:
        glm::ivec2 m_size = glm::ivec2(0);
        std::string m_name;
        bool m_display = true;
    };
}
