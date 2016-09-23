#version 450 core

layout(location = 0) in vec3 vert_position;
layout(location = 1) in vec3 vert_normal;
layout(location = 2) in vec2 vert_tex_coord;

out Frag_data_i {
	vec3 normal;
	vec2 tex_coord;
} vs_out;


void main()
{
	gl_Position = vec4(vert_position, 1);
	vs_out.normal = vert_normal;
	vs_out.tex_coord = vert_tex_coord;
}
