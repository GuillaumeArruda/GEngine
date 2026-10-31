#version 440

uniform mat4 world_matrix;

layout(location = 0) in vec3 in_vertex_pos;

void main()
{
    gl_Position = world_matrix * vec4(in_vertex_pos, 1.0);
}  