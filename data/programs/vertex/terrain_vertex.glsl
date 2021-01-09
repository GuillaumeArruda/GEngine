#version 440

layout(location = 0) in vec3 in_vertex_pos;
layout(location = 2) in vec2 in_uv;

uniform mat4 mvp;
uniform mat4 world_matrix;

out vec3 f_world_pos;
out vec2 f_uv;


void main()
{
	gl_Position = vec4(in_vertex_pos, 1.0);
	f_world_pos = (world_matrix * vec4(in_vertex_pos, 1.0)).xyz;
	f_uv = in_uv;
}