#include "stdafx.h"

#include "gmath/units.h"

#include "gserializer/serializer.h"
#include "gserializer/gmath_serialization.h"

void gserializer::process(serializer& serializer, glm::vec3& vector)
{
    serializer.process("x", vector.x);
    serializer.process("y", vector.y);
    serializer.process("z", vector.z);
}

void gserializer::process(serializer& serializer, glm::vec4& vector)
{
    serializer.process("x", vector.x);
    serializer.process("y", vector.y);
    serializer.process("z", vector.z);
    serializer.process("w", vector.w);
}

void gserializer::process(serializer& serializer, glm::mat4& matrix)
{
    serializer.process("[0]", matrix[0]);
    serializer.process("[1]", matrix[1]);
    serializer.process("[2]", matrix[2]);
    serializer.process("[3]", matrix[3]);
}

void gserializer::process(serializer& serializer, gmath::radian& radian)
{
    serializer.process("m_value", radian.value());
}

void gserializer::process(serializer& serializer, gmath::degree& degree)
{
    serializer.process("m_value", degree.value());
}
