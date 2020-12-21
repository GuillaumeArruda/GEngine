#version 440

in vec3 tex_coords;

uniform samplerCube cube_map;

out vec4 color;

void main() 
{
	color = texture(cube_map, tex_coords);
}