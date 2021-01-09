#version 440

in smooth vec3 f_world_pos;
in smooth vec2 f_uv;

layout(location = 0) out vec3 diffuse_target;
layout(location = 1) out vec3 position_target;
layout(location = 2) out vec3 normal_target;
layout(location = 3) out vec4 specular_target;

uniform sampler2D height_map;
uniform float max_height;

vec3 compute_normal(vec2 uv_coord)
{
	const ivec2 height_size = textureSize(height_map, 0);
	const float offset_width = 1 / float(height_size.x);
	const float offset_height = 1 / float(height_size.y);
	const float multiplicator = 2 * max_height;
	const float above =  texture(height_map, uv_coord + vec2(0, offset_height)).x;
	const float below =  texture(height_map, uv_coord + vec2(0, -offset_height)).x;
	const float left =   texture(height_map, uv_coord + vec2(offset_width, 0)).x;
	const float right =  texture(height_map, uv_coord + vec2(-offset_width, 0)).x;
	return normalize(vec3(multiplicator * (right - left), 1, multiplicator * (below - above)));
}

void main()
{
	const vec3 normal = compute_normal(f_uv);
	diffuse_target = texture(height_map, f_uv).xyz;
	position_target = f_world_pos;
	normal_target = compute_normal(f_uv);
	specular_target = vec4(1);
}