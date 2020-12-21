#version 440

layout(location = 0) in vec3 in_vertex_pos;

uniform mat4 mvp;

out vec3 tex_coords;

void main()
{
	const vec4 pos = mvp * vec4(in_vertex_pos, 1.0);
	gl_Position = pos.xyww;
	tex_coords = in_vertex_pos;
}