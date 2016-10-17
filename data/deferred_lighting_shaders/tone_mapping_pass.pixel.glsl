#version 450 core

const float a = 2.51f;
const float b = 0.03f;
const float c = 2.43f;
const float d = 0.59f;
const float e = 0.14f;


layout(binding = 0)	uniform sampler2D	u_tex_material_lighting_result;

layout(location = 0) out vec3 rt_ldr_result;


vec3 aces_tone_mapping(vec3 x);

void main()
{
	ivec2 screen_uv = ivec2(gl_FragCoord.xy);
	vec3 hdr = texelFetch(u_tex_material_lighting_result, screen_uv, 0).rgb;
	if (all(lessThan(hdr, vec3(0.0)))) discard;

	rt_ldr_result = aces_tone_mapping(hdr);
}

vec3 aces_tone_mapping(vec3 x)
{
	return clamp((x * (a * x + b)) / (x *(c * x + d) + e), 0.0, 1.0);
}
