#version 440
in smooth vec3 normal;
in smooth vec2 uv;

uniform sampler2D albedo_map;
uniform sampler2D normal_map;

out vec4 frag_color;

void main()
{
    frag_color = texture(albedo_map, uv) + (texture(normal_map, uv) * 2.0 - 1.0);
}