#version 450 core

uniform mat4 g_projection_matrix;
uniform mat4 g_view_matrix;
uniform mat4 g_model_matrix;
uniform uint g_layer_count;
uniform uint g_draws_per_layer;
uniform uint g_layer_index;
uniform uint g_draw_index;
uniform float g_position_step;

layout(location = 0) in vec3 vert_position;
layout(location = 1) in vec3 vert_normal;
layout(location = 2) in vec2 vert_tex_coord;

out VS_result {
	vec3 normal_ws;
	vec2 tex_coord;
	float shadow_factor;
} result;

void main()
{
	mat3 normal_matrix = mat3(g_model_matrix);

	//float offset = float(g_draw_index) / float(g_layer_count * g_draws_per_layer);
	//float offset = min(0.01 * float(g_draw_index), float(g_draw_index) / float(g_layer_count * g_draws_per_layer));
	//float offset = 0.005 * float(g_draw_index);
	vec3 pos = vert_position + vert_normal * (float(g_draw_index) * g_position_step);
	vec4 pos_ws = g_model_matrix * vec4(pos, 1);

	gl_Position = g_projection_matrix * g_view_matrix * pos_ws;
	result.normal_ws = normal_matrix * vert_normal;
	result.tex_coord = vert_tex_coord;
	result.shadow_factor = float(g_draw_index + 1) / float(g_layer_count * g_draws_per_layer);
}
