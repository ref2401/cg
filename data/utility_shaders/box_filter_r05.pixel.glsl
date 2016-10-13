#version 450 core


layout(binding = 0) uniform sampler2D	u_tex_source;
					uniform uvec2		u_filter_direction;


layout(location = 0) out vec4 rt_filter_result;


void main() {
	ivec2 uv = ivec2(gl_FragCoord.xy);
	const ivec2 offset = ivec2(u_filter_direction);
	const ivec2 offset_mul_02 = 2 * offset;
	const ivec2 offset_mul_03 = 3 * offset;
	const ivec2 offset_mul_04 = 4 * offset;

	vec4 accum = vec4(0);
	accum += texelFetch(u_tex_source, uv - offset_mul_04, 0);
	accum += texelFetch(u_tex_source, uv - offset_mul_03, 0);
	accum += texelFetch(u_tex_source, uv - offset_mul_02, 0);
	accum += texelFetch(u_tex_source, uv - offset, 0);
	accum += texelFetch(u_tex_source, uv, 0);
	accum += texelFetch(u_tex_source, uv + offset, 0);
	accum += texelFetch(u_tex_source, uv + offset_mul_02, 0);
	accum += texelFetch(u_tex_source, uv + offset_mul_03, 0);
	accum += texelFetch(u_tex_source, uv + offset_mul_04, 0);

	rt_filter_result = accum / 9.0;
}