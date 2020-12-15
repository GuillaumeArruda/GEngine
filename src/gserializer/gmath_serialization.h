#pragma once


#include "gmath/position.h"
#include "gmath/direction.h"
#include "gmath/transform.h"

namespace gmath
{
    struct radian;
    struct degree;
}

namespace gserializer
{
    struct serializer;
    void process(serializer& serializer, glm::vec3& vector);
    void process(serializer& serializer, glm::vec4& vector);
    void process(serializer& serializer, glm::mat4& matrix);
    void process(serializer& serializer, gmath::radian& radian);
    void process(serializer& serializer, gmath::degree& degree);

    template<class Space>
    void process(serializer& serializer, gmath::position<Space>& position)
    {
        serializer.process("m_position", position.get_internal_position());
    }

    template<class Space>
    void process(serializer& serializer, gmath::direction<Space>& direction)
    {
        auto dir = static_cast<typename gmath::direction<Space>::glm_vector_type>(direction);
        serializer.process("x", dir);
        direction = gmath::direction<Space>(dir);
    }

    template<class SpaceFrom, class SpaceTo>
    void process(serializer& serializer, gmath::transform<SpaceFrom, SpaceTo>& transform)
    {
        serializer.process("m_transform", transform.get_internal_matrix());
    }
}