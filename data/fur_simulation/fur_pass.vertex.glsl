#version 450 core

const float pi = 3.1415926535;

uniform mat4 g_pvm_matrix;
uniform mat4 g_model_matrix;
// xyz: view_position_ws
// w: shell_count
uniform vec4 g_view_position_ws;
// x: curl_radius
// y: curl_frequency
// z: threshold_power
// w: shadow_factor_power
uniform vec4 g_strand_props;

layout(location = 0) in vec3 vert_position;
layout(location = 1) in vec3 vert_strand_rest_position;
layout(location = 2) in vec3 vert_strand_curr_position;
layout(location = 3) in vec3 vert_normal;
layout(location = 4) in vec2 vert_tex_coord;
layout(location = 5) in vec4 vert_tangent_h;


out VS_result {
	vec3 normal_ws;
	vec2 tex_coord;
	float shadow_factor;
	float fur_alpha_factor;
	float fur_mask_threshold;
} result;


vec3 calc_position(float h, vec3 tangent, vec3 bitangent);

void main()
{
	const float h = float(gl_InstanceID + 1) / float(g_view_position_ws.w);
	const vec3 tangent = vert_tangent_h.xyz;
	const vec3 bitangent = vert_tangent_h.w * cross(vert_normal, tangent);
	const vec3 v_pos = calc_position(h, tangent, bitangent);
	
	gl_Position = g_pvm_matrix * vec4(v_pos, 1);

	const mat3 normal_matrix = mat3(g_model_matrix);
	const vec3 pos_ws = (g_model_matrix * vec4(v_pos, 1)).xyz;
	const vec3 norm_ws = normalize(normal_matrix * vert_normal);
	const vec3 dir_to_view_ws = normalize(g_view_position_ws.xyz - pos_ws);

	result.normal_ws = norm_ws;
	result.tex_coord = vert_tex_coord;
	result.shadow_factor = pow(h, g_strand_props.z);
	result.fur_alpha_factor = max(0, dot(norm_ws, dir_to_view_ws));
	result.fur_mask_threshold = pow(h, g_strand_props.w);
}

vec3 calc_position(float h, vec3 tangent, vec3 bitangent)
{
	const float one_minus_h = 1 - h;
	const vec3 p0 = vert_position;
	const vec3 p1 = vert_strand_rest_position;
	const vec3 p2 = vert_strand_curr_position;
	vec3 bent_pos = pow(one_minus_h, 2) * p0
		+ 2 * h * one_minus_h * p1
		+ pow(h, 2) * p2;

	float angle = 2 * pi * h * g_strand_props.y;
	float cos_a = cos(angle);
	float sin_a = sin(angle);
	const vec3 curl_offset = g_strand_props.x * (cos_a * tangent + sin_a * bitangent);

	return bent_pos + curl_offset;
}
