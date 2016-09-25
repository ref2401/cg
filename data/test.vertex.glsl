#version 450 core

uniform mat4 u_pvm_matrix;

layout(location = 0) in vec3 vert_position;
layout(location = 1) in vec3 vert_normal;
layout(location = 2) in vec2 vert_tex_coord;
layout(location = 10) in float draw_index;

out Frag_data_i {
	vec3 normal;
	vec2 tex_coord;
	flat float draw_index;
} vs_out;


void main()
{
	gl_Position = u_pvm_matrix * vec4(vert_position.x + draw_index * 0.2, vert_position.y, vert_position.z, 1);
	vs_out.normal = vert_normal;
	vs_out.tex_coord = vert_tex_coord;
	vs_out.draw_index = draw_index;
}
