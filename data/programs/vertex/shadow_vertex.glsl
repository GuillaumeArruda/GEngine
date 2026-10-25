#version 440

layout(location = 0) in vec3 in_vertex_pos;

uniform mat4 mvp;

void main()
{
    gl_Position = mvp * vec4(in_vertex_pos, 1.0);
}