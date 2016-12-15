#version 450 core

layout(binding = 0) uniform sampler2D g_tex_diffuse_rgb;
layout(binding = 1) uniform sampler2D g_tex_fur_mask;
					uniform vec3 g_light_dir_ws;

in VS_result{
	vec3 normal_ws;
	vec2 tex_coord;
	float shadow_factor;
	float fur_mask_threshold;
} frag;

layout(location = 0) out vec4 rt_color;


void main()
{
	// TODO(ref2401): if filter: linear then use textureGather for the tex_noise
	//const float noise = texture(g_tex_noise, noise_uvw).r;
	const vec3 diffuse_rgb = texture(g_tex_diffuse_rgb, frag.tex_coord).rgb;
	const float fur_alpha = step(frag.fur_mask_threshold, texture(g_tex_fur_mask, frag.tex_coord).r);

	float cosTi = max(0, dot(g_light_dir_ws, normalize(frag.normal_ws)));
	vec3 diffuse_term = cosTi * diffuse_rgb;

	rt_color = vec4(diffuse_term * frag.shadow_factor, fur_alpha);

	/*float v = fur_mask * frag.alpha;
	if (rt_color.x > rt_color.y) {
		rt_color = vec4(v, v, v, 1);
	}
	else {
		rt_color = vec4(v, v, v, 1);
	}*/
}
