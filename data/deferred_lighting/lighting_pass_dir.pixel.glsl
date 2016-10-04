#version 450 core

uniform vec3 u_light_ambient_up_irradiance;
uniform vec3 u_light_ambient_down_irradiance;
layout(binding = 0) uniform sampler2D u_tex_normal_smoothness;

layout(location = 0) out vec3 rt_lighting_ambient_term;


void main()
{
	ivec2 uv = ivec2(gl_FragCoord.xy);
	vec4 normal_smoothness = texelFetch(u_tex_normal_smoothness, uv, 0);
	vec3 normal_vs = normalize(normal_smoothness.xyz);
	float smoothness = normal_smoothness.w;

	// Hemispheric ambient lighting
	rt_lighting_ambient_term = mix(
		u_light_ambient_down_irradiance,
		u_light_ambient_up_irradiance,
		normal_vs.y);
}