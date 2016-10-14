#version 450 core

// assume that:
// GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS = 48
// GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS = 16
// that leaves us 32 texture units for the pixel shader.
//
// batch_size for the pixel shader = 32. draw_call_index is in [0, 32).


uniform float u_arr_smoothness[32];
layout(binding = 0) uniform sampler2D u_arr_tex_normal_map[32];

in Pixel_data_i {
	vec3 normal_vs;
	vec3 tangent_vs;
	vec3 bitanget_vs;
	vec2 tex_coord;
	float depth_vs;
	flat uint draw_call_index;
} ps_in;

layout(location = 0) out vec4 rt_nds; // normal, depth, smoothness


vec2 encode_normal_vs(vec3 normal_vs);

void main()
{
	vec3 normal_vs = normalize(ps_in.normal_vs);
	vec3 tangent_vs = normalize(ps_in.tangent_vs);
	tangent_vs = normalize(tangent_vs - normal_vs * dot(normal_vs, tangent_vs)); // Gram-Schmidt orthogonalize.
	vec3 bitangent_vs = normalize(ps_in.bitanget_vs);
	mat3 view_space_matrix = mat3(tangent_vs, bitangent_vs, normal_vs);

	vec3 normal_ts = texture(u_arr_tex_normal_map[ps_in.draw_call_index], ps_in.tex_coord).xyz;
	normal_ts = 2.0 * normal_ts - 1.0;
	vec3 actual_normal_vs = normalize(view_space_matrix * normal_ts);

	rt_nds = vec4(
		encode_normal_vs(actual_normal_vs),
		ps_in.depth_vs,
		u_arr_smoothness[ps_in.draw_call_index].x);
}

vec2 encode_normal_vs(vec3 normal_vs)
{
	return vec2(normal_vs.xy * 0.5 + 0.5);
}