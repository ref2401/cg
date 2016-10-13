#version 450 core

// 1D-kernel: 0.1784 0.210431 0.222338 0.210431 0.1784
// 0.06136	0.24477	0.38774	0.24477	0.06136
// 0.011254	0.016436 0.023066 0.031105 0.040306 0.050187 0.060049 0.069041 0.076276 0.080977 0.082607 0.080977 0.076276 0.069041 0.060049 0.050187 0.040306 0.031105 0.023066 0.016436 0.011254

// 0.000272337, 0.00089296, 0.002583865, 0.00659813, 0.014869116, 
// 0.029570767, 0.051898313, 0.080381679, 0.109868729, 0.132526984,
// 0.14107424,
// 0.132526984, 0.109868729, 0.080381679, 0.051898313, 0.029570767,
// 0.014869116, 0.00659813, 0.002583865, 0.00089296, 0.000272337

layout(binding = 0) uniform sampler2D	u_tex_source;
					uniform uvec2		u_filter_direction;


layout(location = 0) out vec4 rt_filter_result;


void main() {
	const ivec2 uv = ivec2(gl_FragCoord.xy);
	const ivec2 offset = ivec2(u_filter_direction);
	const ivec2 offset_mul_02 = 2 * offset;
	const ivec2 offset_mul_03 = 3 * offset;
	const ivec2 offset_mul_04 = 4 * offset;
	const ivec2 offset_mul_05 = 5 * offset;
	const ivec2 offset_mul_06 = 6 * offset;
	const ivec2 offset_mul_07 = 7 * offset;
	const ivec2 offset_mul_08 = 8 * offset;
	const ivec2 offset_mul_09 = 9 * offset;
	const ivec2 offset_mul_10 = 10 * offset;

	vec4 accum = vec4(0);
	accum += texelFetch(u_tex_source, uv - offset_mul_10, 0)	* 0.000272337;
	accum += texelFetch(u_tex_source, uv - offset_mul_09, 0)	* 0.00089296;
	accum += texelFetch(u_tex_source, uv - offset_mul_08, 0)	* 0.002583865;
	accum += texelFetch(u_tex_source, uv - offset_mul_07, 0)	* 0.00659813;
	accum += texelFetch(u_tex_source, uv - offset_mul_06, 0)	* 0.014869116;
	accum += texelFetch(u_tex_source, uv - offset_mul_05, 0)	* 0.029570767;
	accum += texelFetch(u_tex_source, uv - offset_mul_04, 0)	* 0.051898313;
	accum += texelFetch(u_tex_source, uv - offset_mul_03, 0)	* 0.080381679;
	accum += texelFetch(u_tex_source, uv - offset_mul_02, 0)	* 0.109868729;
	accum += texelFetch(u_tex_source, uv - offset, 0)			* 0.132526984;
	accum += texelFetch(u_tex_source, uv, 0)					* 0.14107424;
	accum += texelFetch(u_tex_source, uv + offset, 0)			* 0.132526984;
	accum += texelFetch(u_tex_source, uv + offset_mul_02, 0)	* 0.109868729;
	accum += texelFetch(u_tex_source, uv + offset_mul_03, 0)	* 0.080381679;
	accum += texelFetch(u_tex_source, uv + offset_mul_04, 0)	* 0.051898313;
	accum += texelFetch(u_tex_source, uv + offset_mul_05, 0)	* 0.029570767;
	accum += texelFetch(u_tex_source, uv + offset_mul_06, 0)	* 0.014869116;
	accum += texelFetch(u_tex_source, uv + offset_mul_07, 0)	* 0.00659813;
	accum += texelFetch(u_tex_source, uv + offset_mul_08, 0)	* 0.002583865;
	accum += texelFetch(u_tex_source, uv + offset_mul_09, 0)	* 0.00089296;
	accum += texelFetch(u_tex_source, uv + offset_mul_10, 0)	* 0.000272337;

	rt_filter_result = accum;
}
