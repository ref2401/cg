cbuffer Model : register(b0) {
	float4x4 g_model_matrix : packoffset(c0);
};

// ----- Vertex Shader -----

struct Vertex {
	float3 position		: V_POSITION;
	float2 tex_coord	: V_TEX_COORD;
};

struct VS_result {
	float3 position_ws	: CONTROL_POINT_POSITION_WS;
	float2 tex_coord	: CONTROL_POINT_TEX_COORD;
};


VS_result vs_main(in Vertex vertex)
{
	VS_result result;
	result.position_ws = mul(g_model_matrix, float4(vertex.position, 1.0)).xyz;
	result.tex_coord = vertex.tex_coord;

	return result;
}

// ----- Hull Shader -----

struct HS_result_const_func {
	float edge_factors[4]	: SV_TessFactor;
	float inside_factors[2]	: SV_InsideTessFactor;
};

HS_result_const_func hs_main_const_func(InputPatch<VS_result, 12> patch)
{
	static const float factor = 32;

	HS_result_const_func result;
	result.edge_factors[0] = factor;
	result.edge_factors[1] = factor;
	result.edge_factors[2] = factor;
	result.edge_factors[3] = factor;
	result.inside_factors[0] = factor;
	result.inside_factors[1] = factor;

	return result;
}

struct HS_result {
	float3 position_ws	: CONTROL_POINT_POSITION_WS;
	float2 tex_coord	: CONTROL_POINT_TEX_COORD;
};

[domain("quad")]
[partitioning("fractional_odd")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(4)]
[patchconstantfunc("hs_main_const_func")]
HS_result hs_main(InputPatch<VS_result, 12> patch, uint patch_id : SV_OutputControlPointID)
{
	HS_result result;
	result.position_ws = patch[patch_id].position_ws;
	result.tex_coord = patch[patch_id].tex_coord;

	return result;
}

// ----- Domain Shader -----

cbuffer Projection_view : register(b0) {
	float4x4 g_projection_view_matrix : packoffset(c0);
};

struct DS_result {
	float4 position_cs	: SV_Position;
	float2 tex_coord	: FRAG_TEX_COORD;
};

[domain("quad")]
DS_result ds_main(const OutputPatch<HS_result, 4> patch,
	float2 domain_location : SV_DomainLocation,
	HS_result_const_func path_data)
{
	float3 pos_ws = float3(0.0, 0.0, 0.0);
	pos_ws.xz = patch[0].position_ws.xz * (1.0f - domain_location.x) * (1.0f - domain_location.y)
		+ patch[1].position_ws.xz * domain_location.x * (1.0f - domain_location.y)
		+ patch[2].position_ws.xz * (1.0f - domain_location.x) * domain_location.y
		+ patch[3].position_ws.xz * domain_location.x * domain_location.y;

	float2 tex_coord = patch[0].tex_coord * (1.0f - domain_location.x) * (1.0f - domain_location.y)
		+ patch[1].tex_coord * domain_location.x * (1.0f - domain_location.y)
		+ patch[2].tex_coord * (1.0f - domain_location.x) * domain_location.y
		+ patch[3].tex_coord * domain_location.x * domain_location.y;

	DS_result result;
	result.position_cs = mul(g_projection_view_matrix, float4(pos_ws, 1.0f));
	result.tex_coord = tex_coord;
	return result;
}

// ----- Pixel Shader -----

float4 ps_main(in DS_result frag) : SV_Target
{
	return float4(frag.tex_coord, 0.0f, 1.0f);
}
