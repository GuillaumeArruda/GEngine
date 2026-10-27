#version 440

uniform mat4 world_matrix;
uniform mat4 camera_world_matrix;
uniform vec3 light_color;
uniform ivec2 screen_size;
uniform float inner_radius;
uniform float outer_radius;
uniform vec3 ambient_light;

uniform sampler2D diffuse_tex;
uniform sampler2D position_tex;
uniform sampler2D normal_tex;
uniform sampler2D specular_tex;
uniform samplerCubeShadow shadow_tex;

in vec3 world_pos;

out vec4 color;

vec2 CalculateUV()
{
	return gl_FragCoord.xy / screen_size;
}

float compute_shadow(vec3 position)
{
    vec3 light_direction = position - world_matrix[3].xyz;
    const float minZ = 0.1;
    const float max_axis_dist = max(max(abs(light_direction.x), abs(light_direction.y)), abs(light_direction.z));
    float current_depth = ((outer_radius + minZ) / (outer_radius - minZ)) - ((2.0 * outer_radius * minZ) / (outer_radius - minZ)) / max_axis_dist;
    current_depth = current_depth * 0.5 + 0.5;
    const float bias = 0.0001;
    return texture(shadow_tex, vec4(light_direction, current_depth - bias)).r;
}

void main()
{
	const vec2 uv = CalculateUV();
	const vec3 position = texture(position_tex, uv).xyz;
	const vec3 normal = texture(normal_tex, uv).xyz;
	const vec3 diffuse = texture(diffuse_tex,uv).xyz;
	const vec3 specular = texture(specular_tex, uv).xyz;
	
	vec3 light_direction = world_matrix[3].xyz - position;
	const float distance = length(light_direction);
	light_direction /= distance;
	const float light_intensity = min(max((outer_radius - distance) / (outer_radius - inner_radius) , 0.0), 1.0);
	
	const float lambertian = max(dot(light_direction, normal), 0.0);

	const vec3 view_dir = normalize(camera_world_matrix[3].xyz - position);
	const vec3 half_dir = normalize(light_direction + view_dir);
	const float spec_angle = max(dot(half_dir, normal), 0.0);
	const float specular_intensity = lambertian > 0 ? pow(spec_angle, 10) : 0.0;

	const vec3 diffuse_color = lambertian * diffuse;
	const vec3 specular_color =  specular * specular_intensity;
	const vec3 ambient = diffuse * ambient_light * light_intensity;
    const float shadow = compute_shadow(position);
	color = vec4((diffuse_color + specular_color) * shadow * light_color * light_intensity + ambient, 1.0);
}