cbuffer Model : register(cb0) {
	float4x4 g_model_matrix	: packoffset(c0);
};

struct VS_result {
	float4 position_ws	: SV_Position;
	float3 normal_ws	: CONTROL_POINT_NORMAL_WS;
	float2 tex_coord	: CONTROL_POINT_TEX_COORD;
	float3 tangent_ws	: CONTROL_POINT_TANGENT_WS;
	float3 bitangent_ws	: CONTROL_POINT_BITANGENT_WS;
};

// ----- Vertex Shader -----

VS_result vs_main(uint vertex_id : SV_VertexID)
{
	float4 vertex_position = float4(0, 0, 0, 1);
	float2 tex_coord = 0;
	
	switch (vertex_id) {
		case 0:
		case 5:
			vertex_position = float4(-1, -1, 0, 1);
			tex_coord = 0;
			break;

		case 1:
			vertex_position = float4(1, -1, 0, 1);
			tex_coord = float2(1, 0);
			break;

		case 2:
		case 3:
			vertex_position = float4(1, 1, 0, 1);
			tex_coord = 1;
			break;

		case 4:
			vertex_position = float4(-1, 1, 0, 1);
			tex_coord = float2(0, 1);
			break;
	}
	
	const float3x3 normal_matrix = (float3x3)g_model_matrix;
	const float3 normal = float3(0, 0, 1);
	const float3 tangent = float3(1, 0, 0);
	const float3 bitangent = float3(0, 1, 0);

	VS_result result;
	result.position_ws = mul(g_model_matrix, vertex_position);
	result.normal_ws = normalize(mul(normal_matrix, normal));
	result.tangent_ws = normalize(mul(normal_matrix, tangent));
	result.bitangent_ws = normalize(mul(normal_matrix, bitangent));
	result.tex_coord = tex_coord;
	return result;
}

// ----- Hull Shader -----

struct HS_patch_const_func_result {
	float edges[3]		: SV_TessFactor;
	float inside		: SV_InsideTessFactor;
};

struct HS_result {
	float4 position_ws	: SV_Position;
	float3 normal_ws	: CONTROL_POINT_NORMAL_WS;
	float2 tex_coord	: CONTROL_POINT_TEX_COORD;
	float3 tangent_ws	: CONTROL_POINT_TANGENT_WS;
	float3 bitangent_ws	: CONTROL_POINT_BITANGENT_WS;
};

HS_patch_const_func_result hs_const_func_main()
{
	static const float factor = 64;

	HS_patch_const_func_result result;
	result.edges[0] = factor;
	result.edges[1] = factor;
	result.edges[2] = factor;
	result.inside = factor;

	return result;
}

[domain("tri")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("hs_const_func_main")]
HS_result hs_main(InputPatch<VS_result, 3> patch,
	uint control_point_id : SV_OutputControlPointID,
	uint patch_id : SV_PrimitiveID)
{
	HS_result result;
	result.position_ws = patch[control_point_id].position_ws;
	result.normal_ws = patch[control_point_id].normal_ws;
	result.tex_coord = patch[control_point_id].tex_coord;
	result.tangent_ws = patch[control_point_id].tangent_ws;
	result.bitangent_ws = patch[control_point_id].bitangent_ws;

	return result;
}

// ----- Domain Shader -----

cbuffer Projection_view : register(cb0) {
	float4x4 g_projection_view_matrix	: packoffset(c0);
};

Texture2D<float> g_tex_displacement_map	: register(t0);
SamplerState g_sampler					: register(s0);

struct DS_result {
	float4 position_cs	: SV_Position;
	float3 position_ws	: FRAG_POSITION_WS;
	float3 normal_ws	: FRAG_NORMAL_WS;
	float2 tex_coord	: FRAG_TEX_COORD;
	float3 tangent_ws	: FRAG_TANGENT_WS;
	float3 bitangent_ws	: FRAG_BITANGENT_WS;
};

[domain("tri")]
DS_result ds_main(const OutputPatch<HS_result, 3> patch, 
	float3 domain_location : SV_DomainLocation,
	HS_patch_const_func_result patch_data)
{
	const float3 normal_ws = normalize(
		patch[0].normal_ws * domain_location.x
		+ patch[1].normal_ws * domain_location.y
		+ patch[2].normal_ws * domain_location.z);

	const float2 tex_coord = patch[0].tex_coord * domain_location.x
		+ patch[1].tex_coord * domain_location.y
		+ patch[2].tex_coord * domain_location.z;

	const float3 tangent_ws = normalize(
		patch[0].tangent_ws * domain_location.x
		+ patch[1].tangent_ws * domain_location.y
		+ patch[2].tangent_ws * domain_location.z);

	const float3 bitangent_ws = normalize(
		patch[0].bitangent_ws * domain_location.x
		+ patch[1].bitangent_ws * domain_location.y
		+ patch[2].bitangent_ws * domain_location.z);

	float3 pos_ws = patch[0].position_ws.xyz * domain_location.x
		+ patch[1].position_ws.xyz * domain_location.y
		+ patch[2].position_ws.xyz * domain_location.z;

	float displ = 0.5 * g_tex_displacement_map.SampleLevel(g_sampler, tex_coord, 0);
	pos_ws += normal_ws * displ;

	DS_result result;
	result.position_cs = mul(g_projection_view_matrix, float4(pos_ws, 1));
	result.position_ws = pos_ws;
	result.normal_ws = normal_ws;
	result.tex_coord = tex_coord;
	result.tangent_ws = tangent_ws;
	result.bitangent_ws = bitangent_ws;
	return result;
}

// ----- Pixel Shader -----
static float pi = 3.1415926535;

Texture2D<float3> g_tex_normal_map	: register(t0);

float4 ps_main(DS_result frag) : SV_Target
{
	static const float3 direction_to_light = normalize(float3(3, 5, -10));

	// normal mapping
	float3 normal_ts = g_tex_normal_map.SampleLevel(g_sampler, frag.tex_coord, 0);
	float3x3 normal_matrix = transpose(float3x3(
		frag.tangent_ws,
		frag.bitangent_ws,
		frag.normal_ws));

	float3 normal_ws = normalize(mul(normal_matrix, normal_ts));

	float3 diffuse_rgb = 1;
	float cosTi = max(0, dot(normalize(normal_ws), direction_to_light));
	float3 ambient_term = diffuse_rgb * 0.15;
	float3 diffuse_term = diffuse_rgb * cosTi / pi;

	return float4(diffuse_term + ambient_term, 1);
}
