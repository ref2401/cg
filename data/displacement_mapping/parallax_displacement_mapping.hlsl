// ----- Vertex shader -----

cbuffer CB_matrices : register(b0) {
	float4x4 g_projection_view_matrix	: register(c0);
	float4x4 g_model_matrix				: register(c4);
	float4x4 g_normal_matrix			: register(c8);
};

struct Vertex {
	float3 position			: VERT_POSITION_MS;
	float3 normal			: VERT_NORMAL_MS;
	float2 tex_coord		: VERT_TEX_COORD;
	float4 tangent_space	: VERT_TANGENT_SPACE_MS;
};

struct VS_output {
	float4 position_cs		: SV_Position;
	float3 position_ws		: PIXEL_POSITION_WS;
	float3 normal_ws		: PIXEL_NORMAL_WS;
	float3 tangent_space_ws	: PIXEL_TANGENT_WS;
	float2 tex_coord		: PIXEL_TEX_COORD;
};

VS_output vs_main(in Vertex vertex) 
{
	const float4 p_ws = mul(g_model_matrix, float4(vertex.position, 1.0f));
	const float3 tangent_ws = mul(g_model_matrix, float4(vertex.tangent_space.xyz, 1.0f)).xyz;

	VS_output output;
	output.position_cs = mul(g_projection_view_matrix, p_ws);
	output.position_ws = p_ws;
	output.normal_ws = mul(g_normal_matrix, float4(vertex.normal, 1.0f)).xyz;
	output.tangent_space_ws = float4(tangent_ws, vertex.tangent_space.w);
	output.tex_coord = vertex.tex_coord;
	return output;
}

// ----- Pixel shader -----

Texture2D g_tex_diffuse_rgb : register(t0);

SamplerState g_sampler {
	Filter = MIN_MAP_MIP_LINEAR;
	AddressU = WRAP;
	AddressV = WRAP;
};

struct PS_output {
	float4 rt_color_0 : SV_Target0;
};

PS_output ps_main(VS_output pixel)
{
	const float4 diffuse_rgb = g_tex_diffuse_rgb.Sample(g_sampler, pixel.tex_coord);

	PS_output output;
	output.rt_color_0 = diffuse_rgb;
	return output;
}
