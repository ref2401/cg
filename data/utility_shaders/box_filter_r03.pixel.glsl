#version 450 core


layout(binding = 0) uniform sampler2D	u_tex_source;
uniform uvec2		u_filter_direction;


layout(location = 0) out vec4 rt_filter_result;


void main() {
	ivec2 uv = ivec2(gl_FragCoord.xy);
	const ivec2 offset = ivec2(u_filter_direction);
	const ivec2 offset_mul_2 = 2 * offset;

	vec4 accum = vec4(0);
	accum += texelFetch(u_tex_source, uv - offset_mul_2, 0);
	accum += texelFetch(u_tex_source, uv - offset, 0);
	accum += texelFetch(u_tex_source, uv, 0);
	accum += texelFetch(u_tex_source, uv + offset, 0);
	accum += texelFetch(u_tex_source, uv + offset_mul_2, 0);

	rt_filter_result = accum / 5.0;
}