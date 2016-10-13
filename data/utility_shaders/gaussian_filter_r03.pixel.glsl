#version 450 core

// 1D-kernel: 0.1784 0.210431 0.222338 0.210431 0.1784
// 1D-kernel: 0.06136 0.24477 0.38774 0.24477 0.06136
// 0.00135 0.157305 0.68269 0.157305 0.00135
// 0.045302 0.242796 0.423805 0.242796 0.045302

layout(binding = 0) uniform sampler2D	u_tex_source;
					uniform uvec2		u_filter_direction;


layout(location = 0) out vec4 rt_filter_result;


void main() {
	ivec2 uv = ivec2(gl_FragCoord.xy);
	const ivec2 offset = ivec2(u_filter_direction);
	const ivec2 offset_mul_02 = 2 * offset;

	vec4 accum = vec4(0);
	accum += texelFetch(u_tex_source, uv - offset_mul_02, 0)	* 0.045302;
	accum += texelFetch(u_tex_source, uv - offset, 0)			* 0.242796;
	accum += texelFetch(u_tex_source, uv, 0)					* 0.423805;
	accum += texelFetch(u_tex_source, uv + offset, 0)			* 0.242796;
	accum += texelFetch(u_tex_source, uv + offset_mul_02, 0)	* 0.045302;

	rt_filter_result = accum;
}
