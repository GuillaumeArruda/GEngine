#version 440

in smooth vec3 f_world_pos;
in smooth vec3 f_normal;
in smooth vec2 f_uv;

layout(location = 0) out vec3 diffuse_target;
layout(location = 1) out vec3 position_target;
layout(location = 2) out vec3 normal_target;
layout(location = 3) out vec4 specular_target;

uniform sampler2D height_map;
uniform float max_height;

vec3 compute_normal(vec2 uv_coord)
{
	const float above =  textureOffset(height_map, uv_coord, ivec2(0, 1)).x;
	const float below =  textureOffset(height_map, uv_coord, ivec2(0, -1)).x;
	const float left =   textureOffset(height_map, uv_coord, ivec2(1, 0)).x;
	const float right =  textureOffset(height_map, uv_coord, ivec2(-1, 0)).x;
	const float above_left =  textureOffset(height_map, uv_coord, ivec2(1, 1)).x;
	const float below_left =   textureOffset(height_map, uv_coord, ivec2(1, -1)).x;
	const float above_right = textureOffset(height_map, uv_coord, ivec2(-1, 1)).x;
	const float below_right = textureOffset(height_map, uv_coord, ivec2(-1, -1)).x;
	
	const float x = (above_right + 2 * right + below_right - below_left - 2 * left - above_left) * max_height;
	const float y = 8.0;
	const float z = (2 * below + below_left + below_right - 2 * above - above_right - above_left) * max_height;
	
	return normalize(vec3(x,y,z));
}

void main()
{
	diffuse_target = vec3(0.5);
	position_target = f_world_pos;
	normal_target = compute_normal(f_uv);
	specular_target = vec4(10);
}