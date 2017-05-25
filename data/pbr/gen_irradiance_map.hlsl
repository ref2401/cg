// ----- vertex shader -----

cbuffer cb_vertex_shader : register(b0) {
	float4x4 g_view_matrix : packoffset(c0);
};

static const float3 cube_vertices[8] = {
	float3(1.0,	1.0, 1.0),
	float3(-1.0, 1.0, 1.0),
	float3(1.0,	1.0, -1.0),
	float3(-1.0, 1.0, -1.0),
	float3(1.0,	-1.0, 1.0),
	float3(-1.0, -1.0, 1.0),
	float3(-1.0, -1.0, -1.0),
	float3(1.0,	-1.0, -1.0)
};

static const uint cube_indices[14] = { 3, 2, 6, 7, 4, 2, 0, 3, 1, 6, 5, 4, 1, 0 };

struct vertex {
	uint id : SV_VERTEXID;
};

struct vs_output {
	float4 position		: SV_POSITION;
	float3 position_ms	: PIXEL_POSITION_MS;
};

vs_output vs_main(vertex vertex)
{
	const float3 p = cube_vertices[cube_indices[vertex.id]];
	const float4 p_cs = mul(g_view_matrix, float4(p, 1.0f));

	vs_output o;
	o.position = p_cs;
	o.position_ms = p;
	return o;
}

// ----- pixel shader -----

TextureCube g_tex_cube_envmap	: register(t0);
SamplerState g_sampler_state	: register(s0);

struct ps_output {
	float4 rt_color_0 : SV_TARGET0;
};

static const float pi = 3.14159265;
static const float v_2pi = 2 * pi;
static const float v_pi_2 = pi * 0.5;
static const float3 up_origin_ms = float3(0, 1, 0);


ps_output ps_main(vs_output pixel)
{
	const float3 n_ms = float3(1, -1, 1) * normalize(pixel.position_ms);
	const float3 right_ms = cross(up_origin_ms, n_ms);
	const float3 up_ms = cross(n_ms, right_ms);

	float counter = 0.0f;
	float3 irradiance = (float3)0.0f;

	for (float phi = 0.0; phi < v_2pi; phi += 0.025f) {
		const float cos_phi = cos(phi);
		const float sin_phi = sin(phi);

		for (float theta = 0.0; theta < v_pi_2; theta += 0.025f) {
			const float cos_theta = cos(theta);
			const float sin_theta = sin(theta);
			const float3 sample_ts = float3(sin_theta * cos_phi, sin_theta * sin_phi, cos_theta);
			const float3 sample_dir = (sample_ts.x * right_ms) + (sample_ts.y * up_ms) + (sample_ts.z * n_ms);

			irradiance += g_tex_cube_envmap.Sample(g_sampler_state, sample_dir).rgb * cos_theta * sin_theta;
			++counter;
		}
	}

	ps_output o;
	o.rt_color_0 = float4(pi * irradiance * (1.0f / counter), 1.0f);
	return o;
}
