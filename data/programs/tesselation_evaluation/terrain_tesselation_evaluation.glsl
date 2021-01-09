#version 440

layout(triangles, equal_spacing, ccw) in;

in vec3 te_world_pos[];
in vec2 te_uv[];

out vec3 f_world_pos;
out vec2 f_uv;

uniform mat4 mvp;
uniform sampler2D height_map;
uniform float max_height;

void main()
{
	const vec2 uv = gl_TessCoord[0] * te_uv[0] + gl_TessCoord[1] * te_uv[1] + gl_TessCoord[2] * te_uv[2];
	const float height = (texture(height_map, uv).x) * max_height;
	
	const vec3 world_pos = 	gl_TessCoord[0] * te_world_pos[0].xzy +
							gl_TessCoord[1] * te_world_pos[1].xzy +
							gl_TessCoord[2] * te_world_pos[2].xzy +
							vec3(0, height, 0);
	
	gl_Position = mvp * vec4(world_pos, 1.0);
	f_world_pos =  world_pos;
	f_uv = uv;
	
}