#pragma once

#include "gphys/resources/collision_shape.h"

namespace gphys
{
    struct heightfield_shape : collision_shape
    {
        heightfield_shape() = default;
        ~heightfield_shape();

        heightfield_shape(heightfield_shape&&) noexcept;
        heightfield_shape& operator=(heightfield_shape&&);

        heightfield_shape(heightfield_shape const&) = delete;
        heightfield_shape& operator=(heightfield_shape const&) = delete;

        void process(gserializer::serializer & serializer) override;

        bool do_load_async() override;
        bool do_load_sync() override;
        void do_unload() override;
    private:
        GSERIALIZER_DECLARE_SUBCLASS_FACTORY_REGISTRATION()
        std::filesystem::path m_heightfield_path;
    };
}