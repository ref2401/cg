cbuffer Scene : register(cb0) {
	float4x4 g_projection_view_matrix	: packoffset(c0);
};

cbuffer Model : register(cb1) {
	float4x4 g_model_matrix	: packoffset(c0);
}

struct Vertex {
	float3 position : V_POSITION;
};

struct VS_output {
	float4 position_cs	: SV_Position;
	float3 rgb			: FRAG_RGB;
};


VS_output vs_main(Vertex vertex)
{
	float4 pos_ws = mul(g_model_matrix, float4(vertex.position, 1));

	VS_output output;
	output.position_cs = mul(g_projection_view_matrix, pos_ws);
	output.position_cs /= output.position_cs.w;
	output.rgb = float3(1, 0, 0);

	return output;
}


float4 ps_main(VS_output frag) : SV_Target
{
	return float4(frag.rgb, 1);
}