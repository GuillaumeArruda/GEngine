#version 440
layout (triangles, invocations = 5) in;
layout (triangle_strip, max_vertices=18) out;

uniform mat4 light_matrices[6];

void main()
{
    for(int i = 0; i < 3; ++i)
    { 
        gl_Layer = gl_InvocationID;
        gl_Position = light_matrices[gl_InvocationID] * gl_in[i].gl_Position;
        EmitVertex();
    }    
    EndPrimitive();
}  