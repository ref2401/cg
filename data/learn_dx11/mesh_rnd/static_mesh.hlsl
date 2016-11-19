cbuffer Scene : register(cb0) {
	float4x4 g_projection_matrix	: packoffset(c0);
	float4x4 g_view_matrix			: packoffset(c4);
};

cbuffer Model : register(cb1) {
	float4x4 g_model_matrix			: packoffset(c0);
};

static float pi = 3.1415926535;

struct Vertex {
	float3 position : V_POSITION;
	float3 normal	: V_NORMAL;
};

struct VS_output {
	float4 position		: SV_Position;
	float3 position_ws	: POSITION_WS;
	float3 normal_ws	: NORMAL_WS;
};

VS_output vs_main(Vertex vertex)
{
	float4 pos_ws = mul(g_model_matrix, float4(vertex.position, 1));
	float4 pos_cs = mul(mul(g_projection_matrix, g_view_matrix), pos_ws);

	VS_output output;
	output.position = pos_cs;
	output.position_ws = pos_ws.xyz;
	output.normal_ws = mul(g_model_matrix, float4(vertex.normal, 1)).xyz;
	return output;
}


float4 ps_main(VS_output frag) : SV_Target
{
	float3 light_pos_ws = float3(1, 5, 2);
	float3 light_dir_ws = normalize(light_pos_ws - frag.position_ws);
	float cosTi = max(0, dot(frag.normal_ws, light_dir_ws));
	float3 diffuse_rgb = float3(0.3, 0.3, 0.3) * cosTi / pi;

	return float4(diffuse_rgb, 1);
}
