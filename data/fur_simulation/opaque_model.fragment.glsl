#version 450 core

layout(binding = 0) uniform sampler2D g_tex_diffuse_rgb;
					uniform vec3 g_light_dir_ws;

in VS_result{
	vec3 normal_ws;
	vec2 tex_coord;
} frag;


layout(location = 0) out vec4 rt_color;

void main()
{
	vec3 diffuse_rgb = texture(g_tex_diffuse_rgb, frag.tex_coord).rgb;
	float cosTi = max(0, dot(g_light_dir_ws, normalize(frag.normal_ws)));
	rt_color = vec4(diffuse_rgb * cosTi, 1);
}
