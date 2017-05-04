//  ----- vertex shader -----

cbuffer cb_vertex_shader : register(b0) {
	float4x4 g_pvm_matrix		: packoffset(c0);
	float4x4 g_normal_matrix	: packoffset(c4);
};

struct vertex {
	float3 position_ms		: VERT_POSITION_MS;
	float3 normal_ms		: VERT_NORMAL_MS;
	float2 uv				: VERT_UV;
	float4 tangent_space_ms	: VERT_TANGENT_SPACE_MS;
};

struct vs_output {
	float4 position_cs	: SV_POSITION;
	float3 normal_ws	: NORMAL_WS;
	float2 uv			: UV;
};

vs_output vs_main(in vertex vertex)
{
	// tangent space matrix
	const float3 normal_ws = (mul(g_normal_matrix, float4(vertex.normal_ms, 1.0f))).xyz;
	const float3 tangent_ws = (mul(g_normal_matrix, float4(vertex.tangent_space_ms.xyz, 1.0f))).xyz;
	const float3 bitangent_ws = vertex.tangent_space_ms.w * cross(normal_ws, tangent_ws);
	const float3x3 world_to_tbn_matrix = float3x3(tangent_ws, bitangent_ws, normal_ws);

	vs_output o;
	o.position_cs = mul(g_pvm_matrix, float4(vertex.position_ms, 1));
	o.normal_ws = normal_ws;
	o.uv = vertex.uv;
	return o;
}

// ----- pixel shader -----

struct ps_output {
	float4 rt_color_0	: SV_TARGET0;
};

ps_output ps_main(in vs_output pixel)
{
	ps_output o;
	o.rt_color_0 = float4(pixel.normal_ws, 1);
	return o;
}
