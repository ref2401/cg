#version 450 core

// 1D-kernel: 0.1784 0.210431 0.222338 0.210431 0.1784

layout(binding = 0) uniform sampler2D	u_tex_source;
					uniform uvec2		u_filter_direction;


layout(location = 0) out vec4 rtFilterResult;


void main() {
	ivec2 uv = ivec2(gl_FragCoord.xy);
	ivec2 offset = ivec2(u_filter_direction);
	ivec2 offset_mul_2 = 2 * offset;

	vec4 accum = vec4(0);
	accum += texelFetch(u_tex_source, uv - offset_mul_2, 0) * 0.1784;
	accum += texelFetch(u_tex_source, uv - offset, 0)		* 0.210431;
	accum += texelFetch(u_tex_source, uv, 0)				* 0.222338;
	accum += texelFetch(u_tex_source, uv + offset, 0)		* 0.210431;
	accum += texelFetch(u_tex_source, uv + offset_mul_2, 0) * 0.1784;

	rtFilterResult = accum;
}
