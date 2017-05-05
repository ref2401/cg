//  ----- vertex shader -----

cbuffer cb_vertex_shader : register(b0) {
	float4x4 g_pvm_matrix		: packoffset(c0);
	float4x4 g_model_matrix		: packoffset(c4);
	float4x4 g_normal_matrix	: packoffset(c8);
	float3 g_light_dir_to_ws	: packoffset(c12);
	float3 g_view_position_ws	: packoffset(c13);
};

struct vertex {
	float3 position_ms		: VERT_POSITION_MS;
	float3 normal_ms		: VERT_NORMAL_MS;
	float2 uv				: VERT_UV;
	float4 tangent_space_ms	: VERT_TANGENT_SPACE_MS;
};

struct vs_output {
	float4 position_cs			: SV_POSITION;
	float3 h_ts_unnormalized	: H_TS_UNNORMALIZED;
	float cos_theta_d			: COS_THETA_D;
	float2 uv					: UV;
};

vs_output vs_main(in vertex vertex)
{
	// tangent space matrix
	const float3 normal_ws = (mul(g_normal_matrix, float4(vertex.normal_ms, 1.0f))).xyz;
	const float3 tangent_ws = (mul(g_normal_matrix, float4(vertex.tangent_space_ms.xyz, 1.0f))).xyz;
	const float3 bitangent_ws = vertex.tangent_space_ms.w * cross(normal_ws, tangent_ws);
	const float3x3 world_to_tbn_matrix = float3x3(tangent_ws, bitangent_ws, normal_ws);

	// 
	const float3 p_ws = mul(g_model_matrix, float4(vertex.position_ms, 1)).xyz;
	const float3 v_ws = normalize(g_view_position_ws - p_ws);
	const float3 h_ws = normalize(v_ws + g_light_dir_to_ws);

	vs_output o;
	o.position_cs = mul(g_pvm_matrix, float4(vertex.position_ms, 1));
	o.h_ts_unnormalized = mul(world_to_tbn_matrix, h_ws);
	o.cos_theta_d = dot(h_ws, g_light_dir_to_ws);
	o.uv = vertex.uv;
	return o;
}

// ----- pixel shader -----

static const float pi = 3.1415926535f;
static const float at = 0.01f;
static const float ab = 0.01f;

struct ps_output {
	float4 rt_color_0	: SV_TARGET0;
};

float3 fresnel(float cos_theta_d)
{
	const float3 f0 = (float3)0.2;
	return f0 + (1 - f0) * pow(1 - cos_theta_d, 5);
}

float distribution(in float3 h_ts)
{
	const float denom = h_ts.z * h_ts.z + (h_ts.x * h_ts.x / (at * at)) + (h_ts.y * h_ts.y / (ab * ab));
	return 1.0f / (pi * at * ab * denom * denom);
}

ps_output ps_main(in vs_output pixel)
{
	const float3 h_ts = normalize(pixel.h_ts_unnormalized);

	const float3 specular_term = fresnel(pixel.cos_theta_d) * distribution(h_ts);

	ps_output o;
	o.rt_color_0 = float4(specular_term, 1);
	return o;
}
