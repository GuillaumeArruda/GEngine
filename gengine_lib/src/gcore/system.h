#pragma once

namespace gcore
{
    struct world;
    struct system
    {
        virtual ~system() {};

        virtual void connect_to_world(gcore::world& world) {};
    };
}