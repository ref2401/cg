cbuffer Scene : register(cb0) {
	float4x4 g_projection_matrix	: packoffset(c0);
	float4x4 g_view_matrix			: packoffset(c4);
};

cbuffer Model : register(cb1) {
	float4x4 g_model_matrix			: packoffset(c0);
};


struct VS_input {
	float3 position : POSITION;
};

struct VS_output {
	float4 position : SV_Position;
};

VS_output vs_main(VS_input input)
{
	float4 pos_ws = mul(g_model_matrix, float4(input.position, 1));
	float4 pos_cs = mul(mul(g_projection_matrix, g_view_matrix), pos_ws);

	VS_output output;
	output.position = pos_cs / pos_cs.w;
	return output;
}


float4 ps_main(VS_output input) : SV_Target
{
	return float4(0.3, 0.3, 0.3, 1);
}
