#version 450

uniform mat4 g_projection_matrix;
uniform mat4 g_view_matrix;
uniform mat4 g_model_matrix;

layout(location = 0) in vec3 vert_position;
layout(location = 1) in vec3 vert_normal;
layout(location = 2) in vec2 vert_tex_coord;
layout(location = 3) in vec4 vert_tangent_h;
layout(location = 4) in vec3 vert_strand_rest_position;
layout(location = 5) in vec3 vert_strand_curr_position;


out VS_result {
	vec4 strand_p0_cs;
	vec4 strand_p1_cs;
	vec2 tex_coord;
} result;


void main()
{
	gl_Position = g_projection_matrix * (g_view_matrix *  (g_model_matrix * vec4(vert_position, 1)));

	result.strand_p0_cs = gl_Position;
	result.strand_p1_cs = g_projection_matrix * (g_view_matrix *  (g_model_matrix * vec4(vert_strand_curr_position, 1)));
	result.tex_coord = vert_tex_coord;
}
