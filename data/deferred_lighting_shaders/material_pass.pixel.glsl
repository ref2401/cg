#version 450 core

// assume that:
// GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS = 48
// GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS = 16
// that leaves us 32 texture units for the pixel shader.
//
// u_tex_lighting_{ambient/diffuer/specular}_term:	3
// u_tex_shadow_map:								1
// u_arr_tex_diffuse_rgb:							14
// u_arr_tex_specular_intensity:					14
//										total :		32
//										unused:		0
// batch_size for the pixel shader = 14. draw_call_index is in [0, 14).

						uniform	mat4		u_dir_light_projection_matrix;
layout(binding = 0)		uniform sampler2D	u_arr_tex_diffuse_rgb[14];
layout(binding = 14)	uniform sampler2D	u_arr_tex_specular_intensity[14];
layout(binding = 28)	uniform sampler2D	u_tex_lighting_ambient_term;
layout(binding = 29)	uniform sampler2D	u_tex_lighting_deffure_term;
layout(binding = 30)	uniform sampler2D	u_tex_lighting_specular_term;
layout(binding = 31)	uniform sampler2D	u_tex_shadow_map;


in Pixel_data_i {
	vec3 position_dls; // position in the direction light's space.
	vec2 tex_coord;
	flat uint draw_call_index;
} ps_in;


layout(depth_unchanged) out float gl_FragDepth; // force early depth test
layout(location = 0) out vec4 rt_material_ligting_result;


float compute_shadow_factor(vec3 position_dls);

float to_color_space(float red, float exponent);

vec3 to_color_space(vec3 rgb, float exponent);


void main()
{
	ivec2 screen_uv = ivec2(gl_FragCoord.xy);
	
	vec3 ambient_term = texelFetch(u_tex_lighting_ambient_term, screen_uv, 0).rgb;
	vec3 diffuse_term = texelFetch(u_tex_lighting_deffure_term, screen_uv, 0).rgb;
	vec3 specular_term = texelFetch(u_tex_lighting_specular_term, screen_uv, 0).rgb;

	vec3 diffuse_rgb = to_color_space(texture(u_arr_tex_diffuse_rgb[ps_in.draw_call_index], ps_in.tex_coord).rgb, 2.2);
	float specular_intensity = to_color_space(texture(u_arr_tex_specular_intensity[ps_in.draw_call_index], ps_in.tex_coord).r, 2.2);

	vec3 ambient_contrib = ambient_term * diffuse_rgb;
	vec3 diffuse_contrib = diffuse_rgb * diffuse_term;
	vec3 specular_contrib = specular_intensity * specular_term;
	float shadow_factor = compute_shadow_factor(ps_in.position_dls);
	
	vec3 lighting_result = ambient_contrib + shadow_factor * (diffuse_contrib + specular_contrib);
	rt_material_ligting_result = vec4(to_color_space(lighting_result, 0.45), 1);
	
	/*if (rt_material_ligting_result.x > rt_material_ligting_result.y) {
		rt_material_ligting_result = vec4(compute_shadow_factor(ps_in.position_dls), 0, 0, 1);
	}
	else {
		rt_material_ligting_result = vec4(compute_shadow_factor(ps_in.position_dls), 0, 0, 1);
	}*/
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
	
	float diff = depth_dls - m1;
	if (diff > 0) {
		// chebushev upper bound
		float variance = max(m2 - m1 * m1, 0.00002);
		return variance / (variance + pow(diff, 2));
	}
	else {
		return 1;
	}
}

vec3 to_color_space(vec3 rgb, float exponent)
{
	rgb.r = pow(rgb.r, exponent);
	rgb.g = pow(rgb.g, exponent);
	rgb.b = pow(rgb.b, exponent);
	return rgb;
}

float to_color_space(float red, float exponent)
{
	return pow(red, exponent);
}