#version 450 core

uniform sampler2D u_tex_diffuse_rgb_array[3];

out vec4 rt_color;

in Frag_data_i {
	vec3 normal;
	vec2 tex_coord;
	flat uint draw_index;
} frag;


void main()
{
	const vec3 dir_to_light = normalize(vec3(1, 1, 1));
	const vec3 light_intensity = vec3(0.5);

	float cosT = clamp(dot(frag.normal, dir_to_light), 0, 1);
	vec3 diffuse_rgb = (texture(u_tex_diffuse_rgb_array[frag.draw_index], frag.tex_coord).rgb);
	diffuse_rgb.r = pow(diffuse_rgb.r, 2.2);
	diffuse_rgb.g = pow(diffuse_rgb.g, 2.2);
	diffuse_rgb.b = pow(diffuse_rgb.b, 2.2);
	diffuse_rgb /= 3.14159265;
	vec3 color = diffuse_rgb * cosT * light_intensity;

	color.r = pow(color.r, 0.45);
	color.g = pow(color.g, 0.45);
	color.b = pow(color.b, 0.45);
	rt_color = vec4(color, 1);
}
