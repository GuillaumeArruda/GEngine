#version 440

uniform mat4 camera_world_matrix;
uniform vec3 light_color;
uniform vec2 screen_size;
uniform float light_intensity;
uniform vec3 ambient_light;
uniform vec3 direction;

uniform sampler2D diffuse_tex;
uniform sampler2D position_tex;
uniform sampler2D normal_tex;
uniform sampler2D specular_tex;

in vec3 world_pos;

out vec4 color;

vec2 CalculateUV()
{
	return gl_FragCoord.xy / screen_size;
}

void main()
{
	const vec2 uv = CalculateUV();
	const vec3 position = texture(position_tex, uv).xyz;
	const vec3 normal = texture(normal_tex, uv).xyz;
	const vec3 diffuse = texture(diffuse_tex,uv).xyz;
	const vec3 specular = texture(specular_tex, uv).xyz;
	
	vec3 light_direction = normalize(direction);
	
	const float lambertian = max(dot(light_direction, normal), 0.0);

	const vec3 view_dir = normalize(camera_world_matrix[3].xyz - position);
	const vec3 half_dir = normalize(light_direction + view_dir);
	const float spec_angle = max(dot(half_dir, normal), 0.0);
	const float specular_intensity = lambertian > 0 ? pow(spec_angle, 10) : 0.0;

	const vec3 diffuse_color = lambertian * diffuse;
	const vec3 specular_color =  specular * specular_intensity;
	const vec3 ambient = diffuse * ambient_light;
	color = vec4((diffuse_color + specular_color) * light_color + ambient, 1.0);
}