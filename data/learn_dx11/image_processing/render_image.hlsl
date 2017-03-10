// ----- vertex shader -----

cbuffer Matrix_cbuffer : register(b0) {
	float4x4 g_pvm_matrix : packoffset(c0);
};

struct VS_output {
	float4 position_cs : SV_Position;
	float2 tex_coord : PX_TEX_COORD;
};

VS_output vs_main(uint vertex_id : SV_VertexID)
{
	static const float2 positions[4] = { 
		float2(-1.0f, -1.0f), 
		float2(1.0f, -1.0f), 
		float2(-1.0f, 1.0f),
		float2(1.0f, 1.0f)
	};

	static const float2 tex_coords[4] = {
		float2(0.0f, 0.0f),
		float2(1.0f, 0.0f),
		float2(0.0f, 1.0f),
		float2(1.0f, 1.0f)
	};

	const float4 pos = float4(positions[vertex_id], 0.0f, 1.0f);
	
	VS_output output;
	output.position_cs = mul(g_pvm_matrix, pos);
	output.tex_coord = tex_coords[vertex_id];
	return output;
}

// ----- pixel shader -----

SamplerState g_sampler			: register(s0);
Texture2D<float> g_tex_image	: register(t0);

float4 ps_main(VS_output pixel) : SV_Target0
{
	//float3 rgb = g_tex_image.Sample(g_sampler, pixel.tex_coord);
	//return float4(rgb, 1.0f);
	return (float4)1.0f;
}
