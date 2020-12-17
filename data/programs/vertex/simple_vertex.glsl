#version 440
layout(location = 0) in vec3 inVertexPos;
layout(location = 1) in vec3 inNormal;
uniform mat4 MVP;
out vec3 Normal;
void main()
{
    gl_Position = MVP * vec4(inVertexPos, 1.0);
    Normal = inNormal;
}