#version 450 core

layout(binding = 0) uniform sampler2D g_tex_diffuse_rgb;
layout(binding = 1) uniform sampler3D g_tex_noise;
					uniform vec3 g_light_dir_ws;
					uniform uint g_layer_count;
					uniform uint g_layer_index;

in VS_result{
	vec3 normal_ws;
	vec2 tex_coord;
	float shadow_factor;
} frag;


layout(location = 0) out vec4 rt_color;


void main()
{
	const vec3 noise_uvw = vec3(frag.tex_coord, float(g_layer_index + 1) / float(g_layer_count));
	// TODO(ref2401): if filter: linear then use textureGather for the tex_noise
	const float noise = texture(g_tex_noise, noise_uvw).r;
	const vec3 diffuse_rgb = texture(g_tex_diffuse_rgb, frag.tex_coord).rgb;

	float cosTi = max(0, dot(g_light_dir_ws, normalize(frag.normal_ws)));
	vec3 rgb = diffuse_rgb * cosTi * frag.shadow_factor;
	rt_color = vec4(rgb, noise);

	//if (rt_color.x > rt_color.y) {
	//	rt_color = vec4(noise, noise, noise, 1);
	//}
	//else {
	//	rt_color = vec4(noise, noise, noise, 1);
	//}
}
