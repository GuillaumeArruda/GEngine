#pragma once

namespace gcore
{
    struct world;
}

namespace gtool
{
    struct window_manager;
    struct window
    {
        virtual ~window() {};
        virtual void update(gcore::world& world, window_manager& manager) = 0;
        virtual const char* get_name() const = 0;
        virtual bool& get_should_display() = 0;
    };
}