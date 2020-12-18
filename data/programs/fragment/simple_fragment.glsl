#version 440
in smooth vec3 Normal;
out vec4 FragColor;

uniform vec4 color;
uniform float intensity;

void main()
{
    FragColor = intensity * color * abs(vec4(Normal, 1.0));
}