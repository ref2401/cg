#version 450 core

layout(binding = 0) uniform sampler2D g_tex_diffuse_rgb;
layout(binding = 1) uniform sampler2D g_tex_fur_mask;
					uniform vec3 g_light_dir_ws;
					uniform float g_fur_mask_uv_factor;

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
	const vec3 diffuse_rgb = texture(g_tex_diffuse_rgb, frag.tex_coord).rgb;
	const float noise = texture(g_tex_fur_mask, frag.tex_coord * g_fur_mask_uv_factor).r;
	const float fur_alpha = noise * step(frag.fur_mask_threshold, noise);
	const vec3 n = normalize(frag.normal_ws);

	// diffuse term
	float diffuse_term = max(0, 0.75 * dot(n, g_light_dir_ws) + 0.25);

	vec3 rgb = diffuse_term * diffuse_rgb * frag.shadow_factor;
	rt_color = vec4(rgb, fur_alpha);

	//vec3 c = diffuse_term * diffuse_rgb * frag.shadow_factor;
	//if (rt_color.x > rt_color.y) {
	//	rt_color = vec4(c, fur_alpha);
	//}
	//else {
	//	rt_color = vec4(c, fur_alpha);
	//}
}
