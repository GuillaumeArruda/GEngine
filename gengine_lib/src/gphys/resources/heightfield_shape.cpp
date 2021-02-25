#include "stdafx.h"

#include <iostream>
#undef _UNICODE
#include <IL/il.h>
#include <IL/ilu.h>
#define _UNICODE

#include <bullet/btBulletCollisionCommon.h>
#include <bullet/BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h>

#include "gphys/resources/heightfield_shape.h"

#include "gserializer/serializer.h"



GSERIALIZER_DEFINE_SUBCLASS_FACTORY_REGISTRATION(gphys::heightfield_shape)


namespace gphys
{
    heightfield_shape::~heightfield_shape() = default;
    heightfield_shape::heightfield_shape(heightfield_shape&&) noexcept = default;
    heightfield_shape& heightfield_shape::operator=(heightfield_shape&&) = default;

    void heightfield_shape::process(gserializer::serializer & serializer)
    {
        collision_shape::process(serializer);
        serializer.process("heightfield_path", m_heightfield_path);
    }

    bool heightfield_shape::do_load_async()
    {
        ILuint const imageId = ilGenImage();
        ilBindImage(imageId);
        ILboolean const success = ilLoadImage(m_heightfield_path.string().c_str());
        auto error = ilGetError();
        if (!success)
        {
            ilDeleteImage(imageId);
            std::cerr << "Failed to open heightfield file: " << m_heightfield_path << " error: " << error << "\n";
            return false;
        }

        auto const width = ilGetInteger(IL_IMAGE_WIDTH);
        auto const height = ilGetInteger(IL_IMAGE_HEIGHT);
        auto const byte_per_pixel = ilGetInteger(IL_IMAGE_BPP);
        auto const size_of_data = ilGetInteger(IL_IMAGE_SIZE_OF_DATA);
        auto const data = ilGetData();
        iluFlipImage();

        
        m_height_field_data = std::make_unique<unsigned char[]>(size_of_data / byte_per_pixel);
        for (size_t i = 0; i < size_of_data / byte_per_pixel; ++i)
        {
            m_height_field_data[i] = data[i * byte_per_pixel];
        }
        m_shape = std::make_unique<btHeightfieldTerrainShape>(width, height, static_cast<void*>(m_height_field_data.get()), 20.f / 255.f, -126.f, 128.f, 1, PHY_UCHAR, false);

        ilDeleteImage(imageId);
        return true;
    }

    bool heightfield_shape::do_load_sync()
    {
        return true;
    }

    void heightfield_shape::do_unload()
    {
        m_shape.reset();
        m_height_field_data.reset();
    }
}

