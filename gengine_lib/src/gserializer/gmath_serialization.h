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

    void process(serializer& serializer, glm::bvec2& vector);
    void process(serializer& serializer, glm::bvec3& vector);
    void process(serializer& serializer, glm::bvec4& vector);
    void process(serializer& serializer, glm::ivec2& vector);
    void process(serializer& serializer, glm::ivec3& vector);
    void process(serializer& serializer, glm::ivec4& vector);
    void process(serializer& serializer, glm::uvec2& vector);
    void process(serializer& serializer, glm::uvec3& vector);
    void process(serializer& serializer, glm::uvec4& vector);
    void process(serializer& serializer, glm::vec2& vector);
    void process(serializer& serializer, glm::vec3& vector);
    void process(serializer& serializer, glm::vec4& vector);
    void process(serializer& serializer, glm::dvec2& vector);
    void process(serializer& serializer, glm::dvec3& vector);
    void process(serializer& serializer, glm::dvec4& vector);
    void process(serializer& serializer, glm::mat2x2& matrix);
    void process(serializer& serializer, glm::mat2x3& matrix);
    void process(serializer& serializer, glm::mat2x4& matrix);
    void process(serializer& serializer, glm::mat3x2& matrix);
    void process(serializer& serializer, glm::mat3x3& matrix);
    void process(serializer& serializer, glm::mat3x4& matrix);
    void process(serializer& serializer, glm::mat4x2& matrix);
    void process(serializer& serializer, glm::mat4x3& matrix);
    void process(serializer& serializer, glm::mat4x4& matrix);

    void process(serializer& serializer, gmath::radian& radian);
    void process(serializer& serializer, gmath::degree& degree);

    template<class Space>
    void process(serializer& serializer, gmath::position<Space>& position)
    {
        using gserializer::process;
        process(serializer, position.get_internal_position());
    }

    template<class Space>
    void process(serializer& serializer, gmath::direction<Space>& direction)
    {
        using gserializer::process;
        auto dir = static_cast<typename gmath::direction<Space>::glm_vector_type>(direction);
        process(serializer, dir);
        direction = gmath::direction<Space>(dir);
    }

    template<class SpaceFrom, class SpaceTo>
    void process(serializer& serializer, gmath::transform<SpaceFrom, SpaceTo>& transform)
    {
        using gserializer::process;
        process(serializer, transform.get_internal_matrix());
    }
}