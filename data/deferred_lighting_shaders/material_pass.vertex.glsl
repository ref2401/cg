#version 450 core


uniform mat4 u_projection_matrix;
uniform mat4 u_view_matrix;
uniform mat4 u_dir_light_view_matrix;
uniform mat4 u_arr_model_matrix[14];

layout(location = 0) in vec3 vert_position;
layout(location = 2) in vec2 vert_tex_coord;
layout(location = 15) in uint draw_call_index;

out Pixel_data_i {
	vec3 position_dls; // position in the direction light's space.
	float depth_vs;
	vec2 tex_coord;
	flat uint draw_call_index;
} vs_out;


void main()
{
	mat4 model_matrix = u_arr_model_matrix[draw_call_index];
	vec4 position_ws = (model_matrix * vec4(vert_position, 1));
	vec4 position_vs = u_view_matrix * position_ws;

	gl_Position = u_projection_matrix * position_vs;

	vs_out.position_dls = (u_dir_light_view_matrix * position_ws).xyz;
	vs_out.depth_vs = position_vs.z;
	vs_out.tex_coord = vert_tex_coord;
	vs_out.draw_call_index = draw_call_index;
}