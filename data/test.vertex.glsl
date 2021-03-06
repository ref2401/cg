#version 450 core

uniform mat4 u_projection_view_matrix;
uniform mat4 u_model_matrix_array[3];
uniform mat3 u_normal_matrix_array[3];

layout(location = 0) in vec3 vert_position;
layout(location = 1) in vec3 vert_normal;
layout(location = 2) in vec2 vert_tex_coord;
// the last location is reserved by renderer.
layout(location = 15) in uint draw_index; 

out Frag_data_i {
	vec3 normal;
	vec2 tex_coord;
	flat uint draw_index;
} vs_out;


void main()
{
	gl_Position = u_projection_view_matrix * (u_model_matrix_array[draw_index] * vec4(vert_position, 1));
	vs_out.normal = u_normal_matrix_array[draw_index] * vert_normal;
	vs_out.tex_coord = vert_tex_coord;
	vs_out.draw_index = draw_index;
}
