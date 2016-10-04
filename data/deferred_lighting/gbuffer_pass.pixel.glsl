#version 450 core

// assume that:
// GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS = 48
// GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS = 16
// that leaves us 32 texture units for the pixel shader:
// 2 textures: tex_normal_map & tex_specular_intensity are used -> each consumes 16 texture unites

layout(binding = 0) uniform sampler2D u_arr_tex_normal_map[16];
layout(binding = 16) uniform sampler2D u_arr_tex_specular_intensity[16];

in Pixel_data_i {
	vec3 normal_vs;
	vec3 tangent_vs;
	vec3 bitanget_vs;
	vec2 tex_coord;
	flat uint instance_draw_index;
} ps_in;

layout(location = 0) out vec4 rt_normal_smoothness;

void main()
{
	vec3 normal_vs = normalize(ps_in.normal_vs);
	vec3 tangent_vs = normalize(ps_in.tangent_vs);
	vec3 bitangent_vs = normalize(ps_in.bitanget_vs);
	mat3 view_space_matrix = mat3(tangent_vs, bitangent_vs, normal_vs);

	vec3 normal_ts = texture(u_arr_tex_normal_map[ps_in.instance_draw_index], ps_in.tex_coord).xyz;
	normal_ts = 2 * normal_ts - vec3(1);
	
	if (texture(u_arr_tex_specular_intensity[ps_in.instance_draw_index], ps_in.tex_coord).x > 0) {
		rt_normal_smoothness = vec4(normalize(view_space_matrix * normal_ts), 1);
	}
	else {
		rt_normal_smoothness = vec4(normalize(view_space_matrix * normal_ts), 1);
	}
}