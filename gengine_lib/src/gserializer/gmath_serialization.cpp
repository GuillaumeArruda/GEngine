#include "stdafx.h"

#include "gmath/units.h"

#include "gserializer/serializer.h"
#include "gserializer/gmath_serialization.h"


namespace gserializer
{
    void process(serializer& serializer, glm::bvec2& vector)
    {
        serializer.process("x", vector.x);
        serializer.process("y", vector.y);
    }

    void process(serializer& serializer, glm::bvec3& vector)
    {
        serializer.process("x", vector.x);
        serializer.process("y", vector.y);
        serializer.process("z", vector.z);
    }

    void process(serializer& serializer, glm::bvec4& vector)
    {
        serializer.process("x", vector.x);
        serializer.process("y", vector.y);
        serializer.process("z", vector.z);
        serializer.process("w", vector.w);
    }

    void process(serializer& serializer, glm::ivec2& vector)
    {
        serializer.process("x", vector.x);
        serializer.process("y", vector.y);
    }

    void process(serializer& serializer, glm::ivec3& vector)
    {
        serializer.process("x", vector.x);
        serializer.process("y", vector.y);
        serializer.process("z", vector.z);
    }

    void process(serializer& serializer, glm::ivec4& vector)
    {
        serializer.process("x", vector.x);
        serializer.process("y", vector.y);
        serializer.process("z", vector.z);
        serializer.process("w", vector.w);
    }

    void process(serializer& serializer, glm::uvec2& vector)
    {
        serializer.process("x", vector.x);
        serializer.process("y", vector.y);
    }

    void process(serializer& serializer, glm::uvec3& vector)
    {
        serializer.process("x", vector.x);
        serializer.process("y", vector.y);
        serializer.process("z", vector.z);
    }

    void process(serializer& serializer, glm::uvec4& vector)
    {
        serializer.process("x", vector.x);
        serializer.process("y", vector.y);
        serializer.process("z", vector.z);
        serializer.process("w", vector.w);
    }

    void process(serializer& serializer, glm::vec2& vector)
    {
        serializer.process("x", vector.x);
        serializer.process("y", vector.y);
    }

    void process(serializer& serializer, glm::vec3& vector)
    {
        serializer.process("x", vector.x);
        serializer.process("y", vector.y);
        serializer.process("z", vector.z);
    }

    void process(serializer& serializer, glm::vec4& vector)
    {
        serializer.process("x", vector.x);
        serializer.process("y", vector.y);
        serializer.process("z", vector.z);
        serializer.process("w", vector.w);
    }

    void process(serializer& serializer, glm::dvec2& vector)
    {
        serializer.process("x", vector.x);
        serializer.process("y", vector.y);
    }

    void process(serializer& serializer, glm::dvec3& vector)
    {
        serializer.process("x", vector.x);
        serializer.process("y", vector.y);
        serializer.process("z", vector.z);
    }

    void process(serializer& serializer, glm::dvec4& vector)
    {
        serializer.process("x", vector.x);
        serializer.process("y", vector.y);
        serializer.process("z", vector.z);
        serializer.process("w", vector.w);
    }

    void process(serializer& serializer, glm::mat2x2& matrix)
    {
        serializer.process("[0]", matrix[0]);
        serializer.process("[1]", matrix[1]);
    }

    void process(serializer& serializer, glm::mat2x3& matrix)
    {
        serializer.process("[0]", matrix[0]);
        serializer.process("[1]", matrix[1]);
    }

    void process(serializer& serializer, glm::mat2x4& matrix)
    {
        serializer.process("[0]", matrix[0]);
        serializer.process("[1]", matrix[1]);
    }

    void process(serializer& serializer, glm::mat3x2& matrix)
    {
        serializer.process("[0]", matrix[0]);
        serializer.process("[1]", matrix[1]);
        serializer.process("[3]", matrix[2]);
    }

    void process(serializer& serializer, glm::mat3x3& matrix)
    {
        serializer.process("[0]", matrix[0]);
        serializer.process("[1]", matrix[1]);
        serializer.process("[3]", matrix[2]);
    }

    void process(serializer& serializer, glm::mat3x4& matrix)
    {
        serializer.process("[0]", matrix[0]);
        serializer.process("[1]", matrix[1]);
        serializer.process("[3]", matrix[2]);
    }

    void process(serializer& serializer, glm::mat4x2& matrix)
    {
        serializer.process("[0]", matrix[0]);
        serializer.process("[1]", matrix[1]);
        serializer.process("[3]", matrix[2]);
        serializer.process("[4]", matrix[3]);
    }

    void process(serializer& serializer, glm::mat4x3& matrix)
    {
        serializer.process("[0]", matrix[0]);
        serializer.process("[1]", matrix[1]);
        serializer.process("[3]", matrix[2]);
        serializer.process("[4]", matrix[3]);
    }

    void process(serializer& serializer, glm::mat4x4& matrix)
    {
        serializer.process("[0]", matrix[0]);
        serializer.process("[1]", matrix[1]);
        serializer.process("[3]", matrix[2]);
        serializer.process("[4]", matrix[3]);
    }

    void process(serializer& serializer, gmath::radian& radian)
    {
        serializer.process("m_value", radian.value());
    }

    void process(serializer& serializer, gmath::degree& degree)
    {
        serializer.process("m_value", degree.value());
    }

    

}
