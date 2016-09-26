#version 450 core

uniform mat4 u_projection_view_matrix;
uniform mat4 u_model_matrix_array[3];

layout(location = 0) in vec3 vert_position;
layout(location = 1) in vec3 vert_normal;
layout(location = 2) in vec2 vert_tex_coord;
// the last location is reserved by renderer.
layout(location = 15) in uint draw_index; 

out Frag_data_i {
	vec3 normal;
	vec2 tex_coord;
	flat float draw_index;
} vs_out;


void main()
{
	float pos_offset = float(draw_index) * 0.2;
	vec4 p = vec4(vert_position.x + pos_offset, vert_position.y, vert_position.z, 1);

	gl_Position = u_projection_view_matrix * (u_model_matrix_array[draw_index] * p);
	vs_out.normal = vert_normal;
	vs_out.tex_coord = vert_tex_coord;
	vs_out.draw_index = float(draw_index);
}
