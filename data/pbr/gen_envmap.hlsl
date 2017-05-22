// projectcs an equirectangular mat to a cube map

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

Texture2D g_tex_equirectangular_envmap	: register(t0);
SamplerState g_sampler_state			: register(s0);

struct ps_output {
	float4 rt_color_0 : SV_TARGET0;
};

float4 sample_equirectangular_envmap(float3 v)
{
	static const float2 inv_atan = float2 (0.1591, 0.3183);

	const float2 uv = float2(atan2(v.z, v.x), asin(v.y)) * inv_atan + 0.5f;
	return g_tex_equirectangular_envmap.Sample(g_sampler_state, uv);
}

ps_output ps_main(vs_output pixel) 
{
	const float3 v = normalize(pixel.position_ms);
	
	ps_output o;
	o.rt_color_0 = sample_equirectangular_envmap(v);
	return o;
}
