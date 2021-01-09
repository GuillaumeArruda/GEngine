#version 440

layout(vertices = 3) out;

in vec3 f_world_pos[];
in vec2 f_uv[];

out vec3 te_world_pos[];
out vec2 te_uv[];

const int tesselation_level = 1;

void main()
{

    gl_TessLevelOuter[0] = tesselation_level;
    gl_TessLevelOuter[1] = tesselation_level;
    gl_TessLevelOuter[2] = tesselation_level;
	gl_TessLevelOuter[3] = tesselation_level;
    gl_TessLevelInner[0] = tesselation_level;
	gl_TessLevelInner[1] = tesselation_level;
	
	te_world_pos[gl_InvocationID] = f_world_pos[gl_InvocationID];
	te_uv[gl_InvocationID] = f_uv[gl_InvocationID];
}