#version 440

layout(location = 0) in vec3 in_vertex_pos;

void main()
{
	gl_Position = vec4(in_vertex_pos, 0.0);
}