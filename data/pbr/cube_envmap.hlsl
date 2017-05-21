// ----- vertex shader -----

cbuffer cb_vertex_shader : register(b0) {
	float4x4 g_pvm_matrix : packoffset(c0);
};

struct vertex {
	float3 position : VERT_POSITION_MS;
};

struct vs_output {
	float4 position		: SV_POSITION;
	float3 position_ms	: PIXEL_POSITION_MS;
};

vs_output vs_main(vertex vertex)
{
	const float4 p_cs = mul(g_pvm_matrix, float4(vertex.position, 1.0f));

	vs_output o;
	o.position = p_cs;
	o.position_ms = vertex.position;
	return o;
}

// ----- pixel shader -----

TextureCube g_tex_envmap		: register(t0);
SamplerState g_sampler_state	: register(s0);

struct ps_output {
	float4 rt_color_0 : SV_TARGET0;
};

ps_output ps_main(vs_output pixel)
{
	const float3 v = normalize(pixel.position_ms);

	ps_output o;
	o.rt_color_0 = g_tex_envmap.Sample(g_sampler_state, v);
	return o;
}
