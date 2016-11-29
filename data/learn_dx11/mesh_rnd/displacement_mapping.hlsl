cbuffer Model : register(cb0) {
	float4x4 g_model_matrix	: packoffset(c0);
};

cbuffer Projection_view : register(cb1) {
	float4x4 g_projection_view_matrix	: packoffset(c0);
};

struct VS_result {
	float4 position_ws	: SV_Position;
	float2 tex_coord	: FRAG_TEX_COORD;
};

// ----- Vertex Shader -----

VS_result vs_main(uint vertex_id : SV_VertexID)
{
	VS_result result;

	float4 vertex_position = float4(-0.5, -0.5, 0, 1);
	result.tex_coord = float2(0, 0);
	if (vertex_id == 1) {
		vertex_position = float4(0.5, -0.5, 0, 1);
		result.tex_coord = float2(1, 0);
	}
	else if (vertex_id == 2) {
		vertex_position = float4(-0.5, 0.5, 0, 1);
		result.tex_coord = float2(0, 1);
	}
	
	float4 pos_ws = mul(g_model_matrix, vertex_position);
	result.position_ws = mul(g_projection_view_matrix, pos_ws);
	
	return result;
}

// ----- Pixel Shader -----

float4 ps_main(VS_result frag) : SV_Target
{
	return float4(1, 0, 0, 1);
}
