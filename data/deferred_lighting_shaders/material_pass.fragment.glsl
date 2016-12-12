#version 450 core

// assume that:
// GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS = 48
// GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS = 16
// that leaves us 32 texture units for the pixel shader.
//
// u_arr_tex_diffuse_rgb:							13
// u_arr_tex_specular_intensity:					13
// u_tex_lighting_{ambient/diffuer/specular}_term:	3
// u_tex_nds:										1
// u_tex_shadow_map:								1
// u_tex_ssao_map:									1
//										total :		32
//										unused:		0
// batch_size for the pixel shader = 13. draw_call_index is in [0, 13).
const uint batch_size = 13;

layout(binding = 0)		uniform sampler2D	u_arr_tex_diffuse_rgb[batch_size];
layout(binding = 13)	uniform sampler2D	u_arr_tex_specular_intensity[batch_size];
layout(binding = 26)	uniform sampler2D	u_tex_lighting_ambient_term;
layout(binding = 27)	uniform sampler2D	u_tex_lighting_deffure_term;
layout(binding = 28)	uniform sampler2D	u_tex_lighting_specular_term;
layout(binding = 29)	uniform sampler2D	u_tex_nds;
layout(binding = 30)	uniform sampler2D	u_tex_shadow_map;
layout(binding = 31)	uniform sampler2D	u_tex_ssao_map;
						uniform	mat4		u_dir_light_projection_matrix;

in Pixel_data_i {
	vec3 position_dls; // position in the direction light's space.
	float depth_vs;
	vec2 tex_coord;
	flat uint draw_call_index;
} ps_in;

layout(location = 0) out vec4 rt_material_ligting_result;


float compute_shadow_factor(vec3 position_dls);

float change_color_space(float red, float exponent);

vec3 change_color_space(vec3 rgb, float exponent);


void main()
{
	const ivec2 screen_uv = ivec2(gl_FragCoord.xy);
	const vec2 viewport_size = vec2(textureSize(u_tex_lighting_ambient_term, 0));
	const vec2 ssao_map_tex_coord = gl_FragCoord.xy / viewport_size;

	// depth test
	float depth_vs = texelFetch(u_tex_nds, screen_uv, 0).z;
	if (abs(ps_in.depth_vs - depth_vs) > 0.001) discard;

	// material properties
	vec3 diffuse_rgb = change_color_space(texture(u_arr_tex_diffuse_rgb[ps_in.draw_call_index], ps_in.tex_coord).rgb, 2.2);
	float specular_intensity = change_color_space(texture(u_arr_tex_specular_intensity[ps_in.draw_call_index], ps_in.tex_coord).r, 2.2);
	
	// precalculated light terms & shadow factors
	vec3 ambient_term = texelFetch(u_tex_lighting_ambient_term, screen_uv, 0).rgb;
	vec3 diffuse_term = texelFetch(u_tex_lighting_deffure_term, screen_uv, 0).rgb;
	vec3 specular_term = texelFetch(u_tex_lighting_specular_term, screen_uv, 0).rgb;
	float shadow_factor = compute_shadow_factor(ps_in.position_dls);
	float ambient_occlusion_factor = texture(u_tex_ssao_map, ssao_map_tex_coord, 0).r;

	// combine material props with light terms
	vec3 ambient_contrib = diffuse_rgb *  mix(vec3(0), ambient_term, pow(ambient_occlusion_factor, 1.9));
	vec3 diffuse_contrib = diffuse_rgb * diffuse_term;
	vec3 specular_contrib = specular_intensity * specular_term;
	
	// final result (shadow factors are used here)
	vec3 lighting_result = ambient_contrib + shadow_factor * (diffuse_contrib + specular_contrib);
	rt_material_ligting_result = vec4(change_color_space(lighting_result, 0.45), 1);
}

float compute_shadow_factor(vec3 position_dls)
{
	// pos in directional light's clip space
	vec4 pos_dlcs = u_dir_light_projection_matrix * vec4(position_dls, 1);
	vec2 shadow_map_uv = (pos_dlcs.xy / pos_dlcs.w) * 0.5 + 0.5;
	vec2 moments = texture(u_tex_shadow_map, shadow_map_uv).xy;
	
	// check if the pixel is not in shadow
	float m1 = moments.x;
	float m2 = moments.y;
	float depth_dls = -position_dls.z;
	
	if (depth_dls < m1)
		return 1.0;

	float diff = depth_dls - m1;
	float variance = max(0.05, m2 - m1 * m1);
	float p_max = variance / (variance + diff * diff);

	// light bleeding reduction
	return smoothstep(0.15, 1.0, p_max);
}

vec3 change_color_space(vec3 rgb, float exponent)
{
	rgb.r = pow(rgb.r, exponent);
	rgb.g = pow(rgb.g, exponent);
	rgb.b = pow(rgb.b, exponent);
	return rgb;
}

float change_color_space(float red, float exponent)
{
	return pow(red, exponent);
}