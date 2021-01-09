#pragma once

#include "gphys/resources/collision_shape.h"

class btCollisionShape;

namespace gphys
{
    struct mesh_shape : collision_shape
    {
        mesh_shape() = default;
        ~mesh_shape();

        mesh_shape(mesh_shape&&) noexcept;
        mesh_shape& operator=(mesh_shape&&);

        mesh_shape(mesh_shape const&) = delete;
        mesh_shape& operator=(mesh_shape const&) = delete;

        void process(gserializer::serializer& serializer) override;

        bool do_load_async() override;
        bool do_load_sync() override;
        void do_unload() override;
    private:
        GSERIALIZER_DECLARE_SUBCLASS_FACTORY_REGISTRATION();
    };
}