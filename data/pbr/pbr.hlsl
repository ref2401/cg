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
	float4 position_cs	: SV_POSITION;
	float3 n_ms			: PIXEL_N_MS;
	float3 l_ts			: PIXEL_L_TS;
	float3 v_ts			: PIXEL_V_TS;
	float2 uv			: PIXEL_UV;
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
	const float3 v_ws = (g_view_position_ws - p_ws);

	vs_output o;
	o.position_cs = mul(g_pvm_matrix, float4(vertex.position_ms, 1));
	o.n_ms = vertex.normal_ms;
	o.l_ts = mul(world_to_tbn_matrix, g_light_dir_to_ws);
	o.v_ts = mul(world_to_tbn_matrix, v_ws);
	o.uv = vertex.uv;
	return o;
}

// ----- pixel shader -----

static const float pi = 3.1415926535f;
//// silver
//static const float g_material_roughness = 0.01f;
//static const float g_material_anisotropic = 0.0f;
//static const float g_material_metallic = 1.0;
//static const float3 g_material_base_color = float3(0.75f, 0.75f, 0.75f);
//static const float3 g_material_fresnel0 = float3(0.98738f, 0.98738f, 0.98738f);
//// cooper
//static const float g_material_roughness = 0.7f; // [0.03, 1.0]
//static const float g_material_anisotropic = 0.5f;
//static const float g_material_metallic = 0.0;
//static const float3 g_material_base_color = float3(0.85f, 0.53f, 0.4f);
//static const float3 g_material_fresnel0 = float3(0.32267f, 0.32267f, 0.32267f);
// plastic
static const float g_material_roughness = 0.4f; // [0.03, 1.0]
static const float g_material_anisotropic = 0.0f;
static const float g_material_metallic = 0.0;
static const float3 g_material_base_color = float3(1, 1, 1);
static const float3 g_material_fresnel0 = float3(0.7f, 0.7f, 0.7f);


TextureCube g_tex_irradiance_map	: register(t0);
SamplerState g_sampler_state		: register(s0);

struct ps_output {
	float4 rt_color_0	: SV_TARGET0;
	float4 rt_color_1	: SV_TARGET1;
};

float distribution(float3 h_ts, float at, float ab)
{
	const float denom = h_ts.z * h_ts.z 
		+ (h_ts.x * h_ts.x / (at * at))
		+ (h_ts.y * h_ts.y / (ab * ab));

	return 1.0f / (pi * at * ab * denom * denom);
}

float3 ibl_result(float3 pixel_n_ms, float3 f0, float cos_theta_v)
{
	const float3 n_ms = normalize(pixel_n_ms);
	const float3 irradiance = g_tex_irradiance_map.Sample(g_sampler_state, n_ms).rgb;

	const float3 gloss = (float3)(1.0f - g_material_roughness);
	const float3 fresnel = f0 + (max(gloss, f0) - f0) * pow(1.0 - cos_theta_v, 5.0);
	return (1 - fresnel) * (1 - g_material_metallic) * irradiance * g_material_base_color;
}

float masking_shadowing(float3 l_ts, float3 v_ts, float at, float ab, float cos_theta_d)
{
	const float al = ((l_ts.x * l_ts.x * at * at) + (l_ts.y * l_ts.y * ab * ab)) / (l_ts.z * l_ts.z);
	const float av = ((v_ts.x * v_ts.x * at * at) + (v_ts.y * v_ts.y * ab * ab)) / (v_ts.z * v_ts.z);
	
	const float Al = sqrt(1.0f + al * al) * 0.5f - 0.5f;
	const float Av = sqrt(1.0f + av * av) * 0.5f - 0.5f;

	const float heaviside = (cos_theta_d > 0.0f) ? 1.0f : 0.0f;
	return heaviside / (1 + Al + Av);
}

ps_output ps_main(in vs_output pixel)
{
	// roughness & anisotropic to at & ab
	//const float a = g_material_roughness;
	const float a = pow(g_material_roughness, 2);
	//const float a = pow(g_material_roughness * 0.5f + 0.5f, 2);
	float at = a;
	float ab = a;
	if (g_material_anisotropic < 0.0f)
		at = pow(g_material_anisotropic, 2);
	else if (g_material_anisotropic > 0.0f)
		ab = pow(g_material_anisotropic, 2);

	const float3 n_ts = float3(0, 0, 1);
	const float3 l_ts = normalize(pixel.l_ts);
	const float3 v_ts = normalize(pixel.v_ts);
	const float3 h_ts = normalize(l_ts + v_ts);

	const float cos_theta_l = max(0.0, dot(l_ts, n_ts));
	const float cos_theta_v = max(0.0, dot(v_ts, n_ts));
	const float cos_theta_d = max(0.0, dot(l_ts, h_ts)); // same as dot(v_ts, h_ts)

	// specular term
	const float distrib = distribution(h_ts, at, ab);
	const float masking = masking_shadowing(l_ts, v_ts, at, ab, cos_theta_d);
	const float3 f0 = lerp(g_material_fresnel0, g_material_base_color, g_material_metallic);
	const float3 fresnel = f0 + (1 - f0) * pow(1 - cos_theta_d, 5);
	const float3 specular_term = g_material_base_color * fresnel * distrib * masking / (0.001f + 4.0f * cos_theta_l * cos_theta_v);
	
	// diffuse term
	const float3 diffuse_term = (1 - g_material_metallic) * (1 - fresnel) * g_material_base_color / pi;
	float3 final_color = ibl_result(pixel.n_ms, f0, cos_theta_v) + cos_theta_l * (diffuse_term + specular_term);
	float3 temp = final_color;
	// tonemapping & gamma correct
	final_color = final_color / (final_color + 1.0);
	final_color = pow(final_color, 1.0 / 2.2);

	ps_output o;
	o.rt_color_0 = float4(final_color, 1);
	o.rt_color_1 = float4(temp,  1);
	return o;
}
