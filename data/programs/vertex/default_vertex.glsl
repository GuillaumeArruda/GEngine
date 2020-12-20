#version 440
layout(location = 0) in vec3 in_vertex_pos;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_uv;
layout(location = 3) in vec3 in_tangent;
layout(location = 4) in vec3 in_binormal;

uniform mat4 mvp;
uniform mat3 normal_matrix;
uniform mat4 world_matrix;

out vec3 world_pos;
out vec3 normal;
out vec3 tangent;
out vec3 binormal;
out vec4 specular;
out vec2 uv;

void main()
{
    gl_Position = mvp * vec4(in_vertex_pos, 1.0);
    normal = normalize(normal_matrix * in_normal);
	tangent = normalize(normal_matrix * in_tangent);
	binormal = normalize(normal_matrix * in_binormal);
	world_pos = (world_matrix * vec4(in_vertex_pos, 1.0)).xyz;
	uv = in_uv;
}