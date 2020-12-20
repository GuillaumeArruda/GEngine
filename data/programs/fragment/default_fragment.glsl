#version 440

in smooth vec3 world_pos;
in smooth vec3 normal;
in smooth vec3 tangent;
in smooth vec3 binormal;
in smooth vec2 uv;


uniform sampler2D albedo_map;
uniform sampler2D normal_map;
uniform sampler2D specular_map;

layout(location = 0) out vec3 diffuse_target;
layout(location = 1) out vec3 position_target;
layout(location = 2) out vec3 normal_target;
layout(location = 3) out vec4 specular_target;

void main()
{
	mat3 tangent_to_world = mat3(tangent, binormal, normal);

    diffuse_target = texture(albedo_map, uv).xyz;
	position_target = world_pos;
	normal_target = tangent_to_world * normalize(texture(normal_map, uv).xyz * 2.0 - 1.0);
	specular_target = vec4(texture(specular_map, uv).xyz, 1.f);
}