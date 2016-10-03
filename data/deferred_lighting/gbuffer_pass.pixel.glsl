#version 450 core

layout(location = 0) out vec4 rt_normal_smoothness;

in Pixel_data_i {
	vec3 normal_vs;
	vec3 tangent_vs;
	vec3 bitanget_vs;
	vec2 tex_coord;
	flat uint instance_draw_index;
} ps_in;

void main()
{
	rt_normal_smoothness = vec4(normalize(ps_in.normal_vs), 1);
}