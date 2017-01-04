#version 450

layout(binding = 0) uniform sampler2D g_tex_geometry;
layout(binding = 1) uniform sampler2D g_tex_strand_data;

in VS_result{
	vec2 tex_coord;
} frag;

layout(location = 0) out vec4 rt_result;

void main()
{
	const int radius = 3;
	const int diameter = 2 * radius;
	const ivec2 uv_bottom_left = ivec2(0, 0);
	const ivec2 uv_top_right = textureSize(g_tex_geometry, 0);
	const ivec2 uv_origin = ivec2(gl_FragCoord.xy);

	vec3 accum = vec3(0);//texelFetch(g_tex_geometry, uv_origin, 0).rgb;
	for (int ri = 0; ri < diameter; ++ri) {
		for (int ci = 0; ci < diameter; ++ci) {

			ivec2 uv = uv_origin + ivec2(ci - radius, ri - radius);
			if (uv == uv_origin) continue;
			if (any(lessThan(uv, uv_bottom_left)) || any(greaterThan(uv, uv_top_right))) continue;

			vec4 strand_data = texelFetch(g_tex_strand_data, uv, 0);
			if (strand_data.w <= 0) continue;

			vec3 strand_dir = strand_data.xyz;
			vec3 uv_dir = normalize(vec3(uv_origin - uv, 0));
			float cos_t = dot(uv_dir, strand_dir);
			if (isnan(cos_t)) {
				rt_result = vec4(0, 1, 0, 1);
				return;
			}
			
			if (cos_t < 0.0) continue;

			vec3 rgb = texelFetch(g_tex_geometry, uv, 0).rgb;
			accum += mix(accum, rgb, cos_t);
		}
	}

	rt_result = vec4(accum, 1);
}
