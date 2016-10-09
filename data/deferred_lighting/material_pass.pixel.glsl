#version 450 core

// assume that:
// GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS = 48
// GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS = 16
// that leaves us 32 texture units for the pixel shader.
//
// u_tex_lighting_{ambient/diffuer/specular}_term:	3 
// u_arr_tex_diffuse_rgb:							14
// u_arr_tex_specular_rgb:							14
//										total :		31
//										unused:		1
// batch_size for the pixel shader = 14. draw_call_index is in [0, 14).


layout(binding = 0) uniform sampler2D u_arr_tex_diffuse_rgb[14];
layout(binding = 14) uniform sampler2D u_arr_tex_specular_rgb[14];
layout(binding = 28) uniform sampler2D u_tex_lighting_ambient_term;
layout(binding = 29) uniform sampler2D u_tex_lighting_deffure_term;
layout(binding = 30) uniform sampler2D u_tex_lighting_specular_term;


in Pixel_data_i {
	vec2 tex_coord;
	flat uint draw_call_index;
} ps_in;


layout(depth_unchanged) out float gl_FragDepth;
layout(location = 0) out vec4 rt_material_ligting_result;

vec3 to_color_space(vec3 rgb, float exponent);

void main()
{
	ivec2 screen_uv = ivec2(gl_FragCoord.xy);
	vec3 ambient_term = texelFetch(u_tex_lighting_ambient_term, screen_uv, 0).rgb;
	vec3 diffuse_term = texelFetch(u_tex_lighting_deffure_term, screen_uv, 0).rgb;
	vec3 specular_term = texelFetch(u_tex_lighting_specular_term, screen_uv, 0).rgb;

	vec3 diffuse_rgb = to_color_space(texture(u_arr_tex_diffuse_rgb[ps_in.draw_call_index], ps_in.tex_coord).rgb, 2.2);
	vec3 specular_rgb = to_color_space(texture(u_arr_tex_specular_rgb[ps_in.draw_call_index], ps_in.tex_coord).rgb, 2.2);

	vec3 ambient_contrib = ambient_term * diffuse_rgb;
	vec3 diffuse_contrib = diffuse_rgb * diffuse_term;
	vec3 specular_contrib = specular_rgb * specular_term;
	vec3 lighting_result = to_color_space(ambient_contrib + diffuse_contrib + specular_contrib, 0.45);

	rt_material_ligting_result = vec4(lighting_result, 1);
}

vec3 to_color_space(vec3 rgb, float exponent)
{
	rgb.r = pow(rgb.r, exponent);
	rgb.g = pow(rgb.g, exponent);
	rgb.b = pow(rgb.b, exponent);
	return rgb;
}