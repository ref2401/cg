#version 450 core

uniform mat4 g_projection_matrix;
uniform mat4 g_view_matrix;
uniform mat4 g_model_matrix;
uniform uint g_shell_count;


layout(location = 0) in vec3 vert_position;
layout(location = 1) in vec3 vert_normal;
layout(location = 2) in vec2 vert_tex_coord;
layout(location = 3) in vec3 vert_strand_rest_position;
layout(location = 4) in vec3 vert_strand_curr_position;

out VS_result {
	vec3 normal_ws;
	vec2 tex_coord;
	float shadow_factor;
	float fur_mask_threshold;
} result;

void main()
{
	// TODO(ref2401): Put h, and fur_mask_threshold into uniform array or into UBO.
	const float h = float(gl_InstanceID + 1) / float(g_shell_count);
	const float one_minus_h = 1 - h;
	const vec3 p0 = vert_position;
	const vec3 p1 = vert_strand_rest_position;
	const vec3 p2 = vert_strand_curr_position;
	const vec3 pos = pow(one_minus_h, 2) * p0 
		+ 2 * h * one_minus_h * p1
		+ pow(h, 2) * p2;
	
	const vec4 pos_ws = g_model_matrix * vec4(pos, 1);
	gl_Position = g_projection_matrix * g_view_matrix * pos_ws;

	const mat3 normal_matrix = mat3(g_model_matrix);
	result.normal_ws = normal_matrix * vert_normal;
	result.tex_coord = vert_tex_coord;
	result.shadow_factor = pow(h, 1.1);
	result.fur_mask_threshold = pow(h, 0.6);
}
