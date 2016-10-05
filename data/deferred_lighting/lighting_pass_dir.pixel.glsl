#version 450 core

struct Dir_light {
	vec3 dir_to_light_vs;
	vec3 irradiance;
	vec3 ambient_irradiance_up;
	vec3 ambient_irradiance_down;
};

uniform Dir_light u_dir_light;
layout(binding = 0) uniform sampler2D u_tex_normal_smoothness;
layout(binding = 1) uniform sampler2D u_tex_depth_map;

layout(location = 0) out vec3 rt_lighting_ambient_term;
layout(location = 1) out vec3 rt_lighting_diffuse_term;


const float v_1_pi = 1.0 / 3.14159265358979;
vec3 calc_ambient_term(vec3 normal_vs);

void main()
{
	ivec2 uv = ivec2(gl_FragCoord.xy);

	vec4 normal_smoothness = texelFetch(u_tex_normal_smoothness, uv, 0);
	if (all(equal(normal_smoothness.xyz, vec3(0)))) discard;

	vec3 normal_vs = normalize(normal_smoothness.xyz);
	float smoothness = normal_smoothness.w;

	float cosTi = max(0, dot(normal_vs, u_dir_light.dir_to_light_vs));
	vec3 common_term = cosTi * u_dir_light.irradiance;
	
	rt_lighting_ambient_term = calc_ambient_term(normal_vs);
	rt_lighting_diffuse_term = common_term * v_1_pi;
}

vec3 calc_ambient_term(vec3 normal_vs)
{
	// Hemispheric ambient lighting
	float blend_factor = dot(normal_vs, vec3(0, 1, 0)) * 0.5f + 0.5f;
	return 0.7 * mix(u_dir_light.ambient_irradiance_down, u_dir_light.ambient_irradiance_up, blend_factor);
}