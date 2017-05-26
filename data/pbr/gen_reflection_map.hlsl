// ----- vertex shaderd -----

cbuffer cb_vertex_buffer : register(b0) {
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
	uint id	: SV_VERTEXID;
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

cbuffer ps_pixel_shader : register(b0) {
	float g_roughness : packoffset(c0);
};

TextureCube g_tex_cube_envmap	: register(t0);
SamplerState g_sampler_state	: register(s0);

struct ps_output {
	float4 rt_color_0 : SV_TARGET0;
};

static const float pi = 3.1415926535;
static const uint sample_count = 1024;


float2 hammersley(uint i, uint count)
{
	uint bits = count;
	bits = (bits << 16u) | (bits >> 16u);
	bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
	bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
	bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
	bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
	const float van_der_corpus = float(bits) * 2.3283064365386963e-10;
	return float2(float(i) / float(count), van_der_corpus);
}

float3 importance_sample_ggx(float2 xi, float3 n, float roughness)
{
	const float phi = 2.0 * pi * xi.x;
	const float cos_theta = sqrt((1.0 - xi.y) / (1.0 + (roughness * roughness - 1.0) * xi.y));
	const float sin_theta = sqrt(1.0 - cos_theta * cos_theta);

	// from spherical coordinates to cartesian coordinates
	const float3 h_ts = float3(cos(phi) * sin_theta, sin(phi) * sin_theta, cos_theta);
	// from tangent-space vector to world-space sample vector
	const float3 up = abs(n.z) < 0.999 ? float3(0.0, 0.0, 1.0) : float3(1.0, 0.0, 0.0);
	const float3 tangent = normalize(cross(up, n));
	const float3 bitangent = cross(n, tangent);

	return normalize((tangent * h_ts.x) + (bitangent * h_ts.y) + (n * h_ts.z));
}

ps_output ps_main(vs_output pixel)
{
	const float3 n = float3(1, -1, 1) * normalize(pixel.position_ms);
	const float3 r = n;
	const float3 v = n;

	float total_weight = 0.0;
	float3 final_color = (float3)0.0f;

	for (uint i = 0; i < sample_count; ++i) {
		const float2 xi = hammersley(i, sample_count);
		const float3 h = importance_sample_ggx(xi, n, g_roughness);
		const float3 l = normalize(2.0 * dot(v, h) * h - v);

		const float cos_theta_l = max(0.0, dot(n, l));
		if (cos_theta_l > 0.0)
		{
			final_color += g_tex_cube_envmap.Sample(g_sampler_state, l).rgb * cos_theta_l;
			total_weight += cos_theta_l;
		}
	}

	ps_output o;
	o.rt_color_0 = float4(final_color / total_weight, 1);
	return o;
}
