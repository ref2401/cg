#version 450 core


struct Dir_light {
	vec3 direction_to_light_vs;
	vec3 irradiance;
	vec3 ambient_irradiance_up;
	vec3 ambient_irradiance_down;
};

uniform uvec2 u_viewport_size;
uniform mat4 u_inv_projection_matrix;
uniform Dir_light u_dlight;
layout(binding = 0) uniform sampler2D u_tex_normal_smoothness;
layout(binding = 1) uniform sampler2D u_tex_depth_map;

layout(location = 0) out vec3 rt_lighting_ambient_term;
layout(location = 1) out vec3 rt_lighting_diffuse_term;
layout(location = 2) out vec3 rt_lighting_specular_term;


const float v_1_pi = 1.0 / 3.14159265358979;
const float v_1_8pi = v_1_pi / 8.0;

vec3 calc_ambient_term(vec3 normal_vs);

vec3 reconstruct_position_vs(ivec2 screen_uv);


void main()
{
	ivec2 screen_uv = ivec2(gl_FragCoord.xy);
	
	vec4 normal_smoothness = texelFetch(u_tex_normal_smoothness, screen_uv, 0);
	if (all(equal(normal_smoothness.xyz, vec3(0)))) discard;

	// model properties
	vec3 position_vs = reconstruct_position_vs(screen_uv);
	vec3 normal_vs = normalize(normal_smoothness.xyz);
	float smoothness = normal_smoothness.w;

	// diffuse & specular terms 
	float cosTi = max(0, dot(normal_vs, u_dlight.direction_to_light_vs));
	vec3 common_term = cosTi * u_dlight.irradiance;
	
	// diffuse term
	vec3 diffuse_term = common_term * v_1_pi;

	// specular term
	vec3 direction_to_viewpoint_vs = normalize(-position_vs);
	vec3 half_vector_vs = normalize(u_dlight.direction_to_light_vs + direction_to_viewpoint_vs);
	float cosTh = max(0, dot(normal_vs, half_vector_vs));
	vec3 specular_term = (smoothness + 8) * v_1_8pi * pow(cosTh, smoothness) * common_term;
	
	rt_lighting_ambient_term = calc_ambient_term(normal_vs);
	rt_lighting_diffuse_term = diffuse_term;
	rt_lighting_specular_term = specular_term;
}

vec3 calc_ambient_term(vec3 normal_vs)
{
	// Hemispheric ambient lighting
	float blend_factor = dot(normal_vs, vec3(0, 1, 0)) * 0.5f + 0.5f;
	return 0.7 * mix(u_dlight.ambient_irradiance_down, u_dlight.ambient_irradiance_up, blend_factor);
}

vec3 reconstruct_position_vs(ivec2 screen_uv)
{
	// NOTE(ref2401): Should I decrease u_viewport_size by uvec2(1) before dividing gl_FragCoord.xy by it?
	vec2 tex_coord = (gl_FragCoord.xy / u_viewport_size);
	float depth = texelFetch(u_tex_depth_map, screen_uv, 0).r * 2.0 - 1.0;
	vec4 p = u_inv_projection_matrix * vec4(tex_coord * 2.0 - 1.0, depth, 1.0);
	return p.xyz / p.w;
}
