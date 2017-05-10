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
	float3 h_ts					: PIX_H_TS;
	float3 l_ts					: PIX_L_TS;
	float3 v_ts					: PIX_V_TS;
	float2 uv					: PIX_UV;
	float3 debug				: PIX_DEBUG;
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
	o.h_ts = mul(world_to_tbn_matrix, h_ws);
	o.l_ts = mul(world_to_tbn_matrix, g_light_dir_to_ws);
	o.v_ts = mul(world_to_tbn_matrix, v_ws);
	o.uv = vertex.uv;
	o.debug = h_ws;
	return o;
}

// ----- pixel shader -----

static const float pi = 3.1415926535f;
static const float g_material_fresnel0 = 0.2f;
static const float g_material_at = 0.3f;
static const float g_material_ab = 0.5f;

struct ps_output {
	float4 rt_color_0	: SV_TARGET0;
};

float distribution(float3 h_ts, float at, float ab)
{
	const float denom = h_ts.z * h_ts.z 
		+ (h_ts.x * h_ts.x / (at * at))
		+ (h_ts.y * h_ts.y / (ab * ab));

	return 1.0f / (pi * at * ab * denom * denom);
}

float masking_shadowing(float3 l_ts, float3 v_ts, float at, float ab, float cos_theta_d)
{
	const float al = (l_ts.x * l_ts.x * at * at) / (l_ts.z * l_ts.z)
		+ (l_ts.y * l_ts.y * ab * ab) / (l_ts.z * l_ts.z);
	const float av = (v_ts.x * v_ts.x * at * at) / (v_ts.z * v_ts.z)
		+ (v_ts.y * v_ts.y * ab * ab) / (v_ts.z * v_ts.z);
	
	const float Al = sqrt(1.0f + al * al) * 0.5f - 0.5f;
	const float Av = sqrt(1.0f + av * av) * 0.5f - 0.5f;

	const float heaviside = (cos_theta_d > 0.0f) ? 1.0f : 0.0f;
	return heaviside / (1 + Al + Av);
}

ps_output ps_main(in vs_output pixel)
{
	const float3 n_ts = float3(0, 0, 1);
	const float3 h_ts = normalize(pixel.h_ts);
	const float3 l_ts = normalize(pixel.l_ts);
	const float3 v_ts = normalize(pixel.v_ts);
	//const float at = pow(g_material_at * 0.5f + 0.5f, 2);
	//const float ab = pow(g_material_ab * 0.5f + 0.5f, 2);
	const float at = pow(g_material_at, 2);
	const float ab = pow(g_material_ab, 2);
	//const float at = g_material_at;
	//const float ab = g_material_ab;

	const float cos_theta_h = dot(h_ts, n_ts);
	const float cos_theta_l = dot(l_ts, n_ts);
	const float cos_theta_v = dot(v_ts, n_ts);
	const float cos_theta_d = dot(h_ts, l_ts); // same as dot(h_ts, v_ts)

	const float fresnel = g_material_fresnel0 + (1 - g_material_fresnel0) * pow(1 - cos_theta_d, 5);
	const float masking = masking_shadowing(l_ts, v_ts, at, ab, cos_theta_d);
	const float3 specular_term = (float3)1.0f * fresnel * distribution(h_ts, at, ab) * masking / (4.0f * cos_theta_l * cos_theta_v);

	ps_output o;
	o.rt_color_0 = float4(specular_term, 1);
	return o;
}
