#version 450 core

out vec4 rt_color;

in Frag_data_i {
	vec3 normal;
	vec2 tex_coord;
} frag;


void main()
{
	float cosT = clamp(dot(frag.normal, vec3(1, 1, 1)), 0, 1);
	vec3 color = vec3(1) * cosT * vec3(frag.tex_coord.s, frag.tex_coord.t, 1) / 3.14159265;
	rt_color = vec4(color, 1);
	//rt_color = vec4(1, 1, 1, 1);
}
