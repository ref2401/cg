struct VS_result {
	float4 position_ws	: SV_Position;
	float2 tex_coord	: FRAG_TEX_COORD;
};

// ----- Vertex Shader -----

VS_result vs_main(uint vertex_id : SV_VertexID)
{
	VS_result result;
	result.position_ws = float4(0, 0, 0, 1);
	result.tex_coord = float2(0, 0);
	return result;
}

// ----- Pixel Shader -----

float4 ps_main(VS_result frag) : SV_Target
{
	return float4(1, 0, 0, 1);
}
