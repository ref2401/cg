// ----- Vertex shader -----

cbuffer CB_matrix : register(b0) {
	float4x4 g_pvm_matrix : register(c0);
};

struct Vertex {
	float3 position			: VERT_POSITION_MS;
	float3 normal			: VERT_NORMAL_MS;
	float2 tex_coord		: VERT_TEX_COORD;
	float4 tangent_sapce	: VERT_TANGENT_SPACE_MS;
};

struct VS_output {
	float4 position_cs	: SV_Position;
	float2 tex_coord	: PIXEL_TEX_COORD;
};

VS_output vs_main(in Vertex vertex) 
{
	VS_output output;
	output.position_cs = mul(g_pvm_matrix, float4(vertex.position, 1.0f));
	output.tex_coord = vertex.tex_coord;
	return output;
}

// ----- Pixel shader -----

//Texture2D g_tex_height_map : register(t0);
//
//SamplerState g_sampler {
//	Filter = MIN_MAP_MIP_LINEAR;
//	AddressU = WRAP;
//	AddressV = WRAP;
//};

struct PS_output {
	float4 rt_color_0 : SV_Target0;
};

PS_output ps_main(VS_output pixel)
{
	PS_output output;
	output.rt_color_0 = float4(pixel.tex_coord, 0.0f, 1.0f);
	return output;
}
