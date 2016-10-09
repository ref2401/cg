#version 450 core


uniform mat4 u_projection_view_matrix;
uniform mat4 u_arr_model_matrix[14];

layout(location = 0) in vec3 vert_position;
layout(location = 2) in vec2 vert_tex_coord;
layout(location = 15) in uint draw_call_index;

out Pixel_data_i {
	vec2 tex_coord;
	flat uint draw_call_index;
} vs_out;


void main()
{
	mat4 model_matrix = u_arr_model_matrix[draw_call_index];
	gl_Position = u_projection_view_matrix * (model_matrix * vec4(vert_position, 1));

	vs_out.tex_coord = vert_tex_coord;
	vs_out.draw_call_index = draw_call_index;
}