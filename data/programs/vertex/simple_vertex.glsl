#version 440
layout(location = 0) in vec3 in_vertex_pos;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_uv;

uniform mat4 mvp;

out vec3 normal;
out vec2 uv;

void main()
{
    gl_Position = mvp * vec4(in_vertex_pos, 1.0);
    normal = in_normal;
	uv = in_uv;
}