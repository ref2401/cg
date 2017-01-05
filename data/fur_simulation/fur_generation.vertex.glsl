#version 450 core

uniform mat4 g_projection_matrix;
uniform mat4 g_view_matrix;
uniform mat4 g_model_matrix;
uniform vec3 g_view_position_ws;
uniform uint g_shell_count;
uniform uint g_shell_index;
uniform float g_shadow_factor_power;
uniform float g_threshold_power;
uniform float g_curl_radius;
uniform float g_curl_frequency;


layout(location = 0) in vec3 vert_position;
layout(location = 1) in vec3 vert_strand_rest_position;
layout(location = 2) in vec3 vert_strand_curr_position;
layout(location = 3) in vec3 vert_normal;
layout(location = 4) in vec2 vert_tex_coord;
layout(location = 5) in vec4 vert_tangent_h;

out VS_result {
	vec3 normal_ws;
	vec3 tip_dir_ws;
	vec3 view_dir_ws;
	vec2 tex_coord;
	float shadow_factor;
	float fur_mask_threshold;
} result;


vec3 calc_bent_position(float h);

vec3 calc_curliness_offset(float h, vec3 tangent, vec3 bitangent);

void main()
{
	// TODO(ref2401): Put h, and fur_mask_threshold into uniform array or into UBO.
	const float h = float(g_shell_index + 1) / float(g_shell_count);
	const vec3 tangent = vert_tangent_h.xyz;
	const vec3 bitangent = vert_tangent_h.w * cross(vert_normal, tangent);
	const vec3 v_pos = calc_bent_position(h) + calc_curliness_offset(h, tangent, bitangent);
	
	const vec4 pos_ws = g_model_matrix * vec4(v_pos, 1);
	gl_Position = g_projection_matrix * g_view_matrix * pos_ws;

	const mat3 normal_matrix = mat3(g_model_matrix);
	result.normal_ws = normal_matrix * vert_normal;
	result.tip_dir_ws = normal_matrix * (v_pos - vert_position);
	result.view_dir_ws = normalize(g_view_position_ws - pos_ws.xyz);
	result.tex_coord = vert_tex_coord;
	result.shadow_factor = pow(h, g_shadow_factor_power);
	result.fur_mask_threshold = pow(h, g_threshold_power);
}

vec3 calc_bent_position(float h)
{
	const float one_minus_h = 1 - h;
	const vec3 p0 = vert_position;
	const vec3 p1 = vert_strand_rest_position;
	const vec3 p2 = vert_strand_curr_position;
	return pow(one_minus_h, 2) * p0
		+ 2 * h * one_minus_h * p1
		+ pow(h, 2) * p2;
}

vec3 calc_curliness_offset(float h, vec3 tangent, vec3 bitangent)
{
	const float pi = 3.1415926535;
	float angle = 2 * pi * h * g_curl_frequency;
	float cos_a = cos(angle);
	float sin_a = sin(angle);

	return g_curl_radius * (cos_a * tangent + sin_a * bitangent);
}
