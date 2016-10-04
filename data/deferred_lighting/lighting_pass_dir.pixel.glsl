#version 450 core

uniform vec3 u_light_ambient_up_irradiance;
uniform vec3 u_light_ambient_down_irradiance;
layout(binding = 0) uniform sampler2D u_tex_normal_smoothness;
layout(binding = 1) uniform sampler2D u_tex_depth_map;

layout(location = 0) out vec3 rt_lighting_ambient_term;


vec3 calc_ambient_term(vec3 normal_vs);

void main()
{
	ivec2 uv = ivec2(gl_FragCoord.xy);

	vec4 normal_smoothness = texelFetch(u_tex_normal_smoothness, uv, 0);
	if (all(equal(normal_smoothness.xyz, vec3(0)))) discard;

	vec3 normal_vs = normalize(normal_smoothness.xyz);
	float smoothness = normal_smoothness.w;

	
	rt_lighting_ambient_term = calc_ambient_term(normal_vs);
}

vec3 calc_ambient_term(vec3 normal_vs)
{
	// Hemispheric ambient lighting
	float blend_factor = dot(normal_vs, vec3(0, 1, 0)) * 0.5f + 0.5f;
	return 0.7 * mix(u_light_ambient_down_irradiance, u_light_ambient_up_irradiance, blend_factor);
}