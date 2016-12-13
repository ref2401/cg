#version 450 core

layout(binding = 0) uniform sampler2D g_tex_diffuse_rgb;
layout(binding = 1) uniform sampler2D g_tex_noise;
					uniform vec3 g_light_dir_ws;

in VS_result{
	vec3 normal_ws;
	vec2 tex_coord;
	float shadow_factor;
} frag;


layout(location = 0) out vec4 rt_color;


void main()
{
	vec3 diffuse_rgb = texture(g_tex_diffuse_rgb, frag.tex_coord).rgb;
	float noise = texture(g_tex_noise, frag.tex_coord).r;

	float cosTi = max(0, dot(g_light_dir_ws, normalize(frag.normal_ws)));
	vec3 rgb = diffuse_rgb * cosTi * frag.shadow_factor;
	rt_color = vec4(rgb, noise);

	/*if (rt_color.x > rt_color.y) {
		rt_color = vec4(diffuse_rgb * cosTi * frag.offset, noise);
	}
	else {
		rt_color = vec4(diffuse_rgb * cosTi * frag.offset, noise);
	}*/
}
