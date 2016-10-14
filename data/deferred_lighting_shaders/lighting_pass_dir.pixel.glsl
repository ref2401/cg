#version 450 core


struct Dir_light {
	vec3 direction_to_light_vs;
	vec3 irradiance;
	vec3 ambient_irradiance_up;
	vec3 ambient_irradiance_down;
};

layout(binding = 0)	uniform		sampler2D	u_tex_nds;
layout(binding = 1)	uniform		sampler2D	u_tex_depth_map;
					uniform		uvec2		u_viewport_size;
					uniform		mat4		u_inv_projection_matrix;
					uniform		Dir_light	u_dlight;

in Pixel_data_i {
	vec3 view_direction_vs;
} ps_in;

layout(location = 0) out vec3 rt_lighting_ambient_term;
layout(location = 1) out vec3 rt_lighting_diffuse_term;
layout(location = 2) out vec3 rt_lighting_specular_term;
layout(location = 3) out vec3 rt_shadow_occlusion_map;


const float v_1_pi = 1.0 / 3.14159265358979;
const float v_1_8pi = v_1_pi / 8.0;


vec3 calc_ambient_term(vec3 normal_vs);

vec3 decode_normal_vs(vec2 encoded_normal_vs);

vec3 reconstruct_position_vs(float depth_vs);

void main()
{
	ivec2 screen_uv = ivec2(gl_FragCoord.xy);

	// model properties
	vec4 nds = texelFetch(u_tex_nds, screen_uv, 0);
	vec3 normal_vs = decode_normal_vs(nds.xy);
	float depth_vs = nds.z;
	float smoothness = nds.w;
	vec3 position_vs = reconstruct_position_vs(depth_vs);

	if (all(equal(normal_vs.xyz, vec3(0)))) discard;

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
	rt_shadow_occlusion_map = vec3(0, 0, 0);
}

vec3 calc_ambient_term(vec3 normal_vs)
{
	// Hemispheric ambient lighting
	float blend_factor = dot(normal_vs, vec3(0, 1, 0)) * 0.5f + 0.5f;
	return 0.7 * mix(u_dlight.ambient_irradiance_down, u_dlight.ambient_irradiance_up, blend_factor);
}

vec3 decode_normal_vs(vec2 encoded_normal_vs)
{
	vec3 normal_vs = vec3(encoded_normal_vs * 2.0 - 1.0, 0);
	normal_vs.z = sqrt(1.0 - dot(normal_vs.xy, normal_vs.xy));
	return normalize(normal_vs);
}

vec3 reconstruct_position_vs(float depth_vs)
{
	// NOTE(ref2401): Should I decrease u_viewport_size by uvec2(1) before dividing gl_FragCoord.xy by it?
	return (depth_vs * ps_in.view_direction_vs) / ps_in.view_direction_vs.z;
}
