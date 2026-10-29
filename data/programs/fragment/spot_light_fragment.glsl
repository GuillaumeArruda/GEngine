#version 440

uniform mat4 camera_world_matrix;
uniform vec3 light_color;
uniform ivec2 screen_size;
uniform vec3 ambient_light;
uniform float inner_angle;
uniform float outer_angle;
uniform float inner_distance;
uniform float outer_distance;

uniform mat4 light_world_matrix;

uniform sampler2D diffuse_tex;
uniform sampler2D position_tex;
uniform sampler2D normal_tex;
uniform sampler2D specular_tex;
uniform sampler2DShadow shadow_tex;

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
	
    const vec3 light_forward = normalize(-light_world_matrix[2].xyz); 
	const vec3 light_pos = light_world_matrix[3].xyz;
    const vec3 frag_to_light = light_pos - position;
	const vec3 normalized_frag_to_light = normalize(frag_to_light);
	const vec3 light_to_frag = -normalized_frag_to_light;
    
    const float theta = dot(light_to_frag, light_forward);
    const float rad_inner = cos(inner_angle * 3.14159265 / 180.0);
	const float rad_outer = cos(outer_angle * 3.14159265 / 180.0);
    const float light_intensity = smoothstep(rad_outer, rad_inner, theta);
    
    const float distance_to_light = length(frag_to_light);
	const float attenuation = smoothstep(outer_distance, inner_distance, distance_to_light);
    
	const float lambertian = max(dot(normalized_frag_to_light, normal), 0.0);
	const vec3 view_dir = normalize(camera_world_matrix[3].xyz - position);
	const vec3 half_dir = normalize(normalized_frag_to_light + view_dir);
	const float spec_angle = max(dot(half_dir, normal), 0.0);
	const float specular_intensity = lambertian > 0.0 ? pow(spec_angle, 10.0) : 0.0;

    const float final_factor = light_intensity * attenuation;

	const vec3 diffuse_color = lambertian * diffuse;
	const vec3 specular_color =  specular * specular_intensity;
	const vec3 ambient = diffuse * ambient_light * final_factor;
  
	color = vec4((diffuse_color + specular_color) * light_color * final_factor + ambient, 1.0);
}