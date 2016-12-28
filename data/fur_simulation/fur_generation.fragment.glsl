#version 450 core

layout(binding = 0) uniform sampler2D g_tex_diffuse_rgb;
layout(binding = 1) uniform sampler2D g_tex_fur_mask;
					uniform vec3 g_light_dir_ws;
					uniform float g_specular_factor;
					uniform float g_specular_power;
					uniform float g_tex_coord_factor;

in VS_result{
	vec3 normal_ws;
	vec3 tip_dir_ws;
	vec3 view_dir_ws;
	vec2 tex_coord;
	float shadow_factor;
	float fur_mask_threshold;
} frag;

layout(location = 0) out vec4 rt_color;


void main()
{
	// TODO(ref2401): if filter: linear then use textureGather for the tex_noise
	//const float noise = texture(g_tex_noise, noise_uvw).r;
	const vec3 n = normalize(frag.normal_ws);
	const vec2 uv = frag.tex_coord * g_tex_coord_factor;
	const float noise = texture(g_tex_fur_mask, uv).r;
	const vec3 diffuse_rgb = texture(g_tex_diffuse_rgb, uv).rgb;
	const float fur_alpha = noise * step(frag.fur_mask_threshold, noise);

	// diffuse term
	float diffuse_term = max(0, 0.75 * dot(n, g_light_dir_ws) + 0.25);

	// specular term
	vec3 half_vector_ws = normalize(g_light_dir_ws + frag.view_dir_ws);

	float cos_th = dot(n, half_vector_ws);
	float sin_th = sqrt(1.0 - pow(cos_th, 2));
	float specular_term = step(0.00001, cos_th) * g_specular_factor * pow(sin_th, g_specular_power);

	vec3 rgb = (diffuse_term + specular_term) * diffuse_rgb * frag.shadow_factor;
	rt_color = vec4(rgb, fur_alpha);

	/*float v = specular_term;
	if (rt_color.x > rt_color.y) {
		rt_color = vec4(v, v, v, fur_alpha);
	}
	else {
		rt_color = vec4(v, v, v, fur_alpha);
	}*/
}
