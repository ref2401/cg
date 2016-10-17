#version 450 core


const uint sample_count = 8;
const uint random_normal_count = 8;
const float sample_depth_limit = 1.5;

layout(binding = 0)	uniform	sampler2D					u_tex_nds;
					uniform	vec3[sample_count]			u_arr_sample_ray;
					uniform vec3[random_normal_count]	u_arr_random_normal;

in Pixel_data_i {
	vec2 tex_coord;
} ps_in;

layout(location = 0) out vec4 rt_ssao_map;

vec3 decode_normal_vs(vec2 encoded_normal_vs);

vec3 get_random_normal_vs();

void main()
{
	vec4 nds = texture(u_tex_nds, ps_in.tex_coord, 0);
	if (nds.w <= 0.01) discard;
	
	float depth_vs = nds.z;
	vec3 normal_vs = decode_normal_vs(nds.xy);
	vec3 rnd_normal_vs = get_random_normal_vs();
	const float sample_ray_scale = abs(rnd_normal_vs.x + rnd_normal_vs.y + rnd_normal_vs.z) / 30;

	float occlusion = 0;
	for (uint i = 0; i < sample_count; ++i) {
		vec3 sample_ray = reflect(u_arr_sample_ray[i], rnd_normal_vs);

		float cos_a = dot(sample_ray, normal_vs);
		if (abs(cos_a) < 0.15) continue;
		if (cos_a < 0)
			sample_ray *= -1;
		
		sample_ray *= sample_ray_scale;

		vec2 sample_uv = ps_in.tex_coord + sample_ray.xy;
		float sample_depth_vs = texture(u_tex_nds, sample_uv).z;
		
		if (abs(depth_vs - sample_depth_vs) > sample_depth_limit) continue;

		// the actual check is if sample_depth_vs is further than modified_depth_vs then continue.
		// if (sample_depth_vs > modified_depth_vs) then continue;
		// but all the depth values are negative in the view space, so the check is inverted.
		float modified_depth_vs = depth_vs + abs(sample_ray.z);
		if ((sample_depth_vs <= modified_depth_vs)) continue;

		occlusion += 1;
	}

	//rt_ssao_map = vec4(occlusion / sample_ray_count, 0, 0, 1);
	float ao_factor = 1 - (occlusion / sample_count);
	ao_factor = mix(0.2, ao_factor, 1.0);
	
	rt_ssao_map = vec4(ao_factor, 0, 0, 1);
}

vec3 decode_normal_vs(vec2 encoded_normal_vs)
{
	vec3 normal_vs = vec3(encoded_normal_vs * 2.0 - 1.0, 0);
	normal_vs.z = sqrt(1.0 - dot(normal_vs.xy, normal_vs.xy));
	return normalize(normal_vs);
}

vec3 get_random_normal_vs()
{
	ivec2 screen_uv = ivec2(gl_FragCoord.xy);
	uint index = (30 * screen_uv.y ^ screen_uv.x + 10 * screen_uv.x * screen_uv.y) % 8;
	return u_arr_random_normal[index];
}