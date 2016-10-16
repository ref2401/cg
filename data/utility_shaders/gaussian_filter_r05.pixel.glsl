#version 450 core

// 0.000006	0.000883 0.029507 0.235589 0.468029 0.235589 0.029507 0.000883 0.000006
// 0.028532 0.067234 0.124009 0.179044 0.20236 0.179044 0.124009 0.067234 0.028532


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
	accum += texelFetch(u_tex_source, uv - offset_mul_04, 0)	* 0.028532;
	accum += texelFetch(u_tex_source, uv - offset_mul_03, 0)	* 0.067234;
	accum += texelFetch(u_tex_source, uv - offset_mul_02, 0)	* 0.124009;
	accum += texelFetch(u_tex_source, uv - offset, 0)			* 0.179044;
	accum += texelFetch(u_tex_source, uv, 0)					* 0.20236;
	accum += texelFetch(u_tex_source, uv + offset, 0)			* 0.179044;
	accum += texelFetch(u_tex_source, uv + offset_mul_02, 0)	* 0.124009;
	accum += texelFetch(u_tex_source, uv + offset_mul_03, 0)	* 0.067234;
	accum += texelFetch(u_tex_source, uv + offset_mul_04, 0)	* 0.028532;

	rt_filter_result = accum;
}
