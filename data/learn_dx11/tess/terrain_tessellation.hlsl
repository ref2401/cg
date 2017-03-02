// ----- Vertex Shader -----

struct Vertex {
	float3 position		: V_POSITION;
	float2 tex_coord	: V_TEX_COORD;
};

struct VS_result {
	float3 position_ms	: CONTROL_POINT_POSITION_MS;
	float2 tex_coord	: CONTROL_POINT_TEX_COORD;
};


VS_result vs_main(in Vertex vertex)
{
	VS_result result;
	result.position_ms = vertex.position;
	result.tex_coord = vertex.tex_coord;

	return result;
}

// ----- Hull Shader -----

cbuffer Tess_control_cbuffer :register(b0) {
	float3 g_camera_position_ms	: packoffset(c0);
	float2 g_distance_min_max	: packoffset(c1);
	float2 g_lod_min_max		: packoffset(c2);
};

struct HS_result_const_func {
	float edge_factors[4]	: SV_TessFactor;
	float inside_factors[2]	: SV_InsideTessFactor;
};

float3 middle_position(float3 p0, float3 p1, float3 p2, float3 p3)
{
	return 0.25 * (p0 + p1 + p2 + p3);
}

float patch_lod(float3 middle_position_ws)
{
	const float d = distance(g_camera_position_ms, middle_position_ws);
	const float d_normalized = saturate((d - g_distance_min_max.x) / (g_distance_min_max.y - g_distance_min_max.x));
	return lerp(g_lod_min_max.x, g_lod_min_max.y,  pow((1.0f - d_normalized), 6));
}

HS_result_const_func hs_main_const_func(InputPatch<VS_result, 12> patch)
{
	// patches' middle positions
	const float3 center_ms = middle_position(patch[0].position_ms, patch[1].position_ms, patch[2].position_ms, patch[3].position_ms);
	const float3 left_ms = middle_position(patch[0].position_ms, patch[2].position_ms, patch[4].position_ms, patch[5].position_ms);
	const float3 bottom_ms = middle_position(patch[0].position_ms, patch[1].position_ms, patch[6].position_ms, patch[7].position_ms);
	const float3 right_ms = middle_position(patch[1].position_ms, patch[3].position_ms, patch[8].position_ms, patch[9].position_ms);
	const float3 top_ms = middle_position(patch[2].position_ms, patch[3].position_ms, patch[10].position_ms, patch[11].position_ms);

	// patches' lod/tess factors
	const float center_lod = patch_lod(center_ms);
	const float left_lod = patch_lod(left_ms);
	const float bottom_lod = patch_lod(bottom_ms);
	const float right_lod = patch_lod(right_ms);
	const float top_lod = patch_lod(top_ms);

	HS_result_const_func result;
	result.edge_factors[0] = min(left_lod, center_lod);
	result.edge_factors[1] = min(bottom_lod, center_lod);
	result.edge_factors[2] = min(right_lod, center_lod);
	result.edge_factors[3] = min(top_lod, center_lod);
	result.inside_factors[0] = center_lod;
	result.inside_factors[1] = center_lod;
	
	//HS_result_const_func result;
	//result.edge_factors[0] = left_lod;
	//result.edge_factors[1] = bottom_lod;
	//result.edge_factors[2] = right_lod;
	//result.edge_factors[3] = left_lod;
	//result.inside_factors[0] = center_lod;
	//result.inside_factors[1] = center_lod;

	//static const float factor = 1;
	//HS_result_const_func result;
	//result.edge_factors[0] = factor;
	//result.edge_factors[1] = factor;
	//result.edge_factors[2] = factor;
	//result.edge_factors[3] = factor;
	//result.inside_factors[0] = factor;
	//result.inside_factors[1] = factor;

	return result;
}

struct HS_result {
	float3 position_ms	: CONTROL_POINT_POSITION_MS;
	float2 tex_coord	: CONTROL_POINT_TEX_COORD;
};

[domain("quad")]
[partitioning("fractional_odd")]
[outputtopology("triangle_ccw")]
[outputcontrolpoints(4)]
[patchconstantfunc("hs_main_const_func")]
HS_result hs_main(InputPatch<VS_result, 12> patch, uint patch_id : SV_OutputControlPointID)
{
	HS_result result;
	result.position_ms = patch[patch_id].position_ms;
	result.tex_coord = patch[patch_id].tex_coord;

	return result;
}

// ----- Domain Shader -----

cbuffer Projection_view : register(b0) {
	float4x4 g_pvm_matrix : packoffset(c0);
};

struct DS_result {
	float4 position_cs	: SV_Position;
	float2 tex_coord	: FRAG_TEX_COORD;
	float3 debug_vec	: FRAG_DEBUG_VEC;
};

[domain("quad")]
DS_result ds_main(const OutputPatch<HS_result, 4> patch,
	float2 domain_location : SV_DomainLocation,
	HS_result_const_func path_data)
{
	float3 pos_ms = float3(0.0, 0.0, 0.0);
	pos_ms.xz = patch[0].position_ms.xz * domain_location.x * (1.0f - domain_location.y)
		+ patch[1].position_ms.xz * (1.0f - domain_location.x) * (1.0f - domain_location.y)
		+ patch[2].position_ms.xz * domain_location.x * domain_location.y
		+ patch[3].position_ms.xz * (1.0f - domain_location.x) * domain_location.y;

	float2 tex_coord = patch[0].tex_coord * domain_location.x * (1.0f - domain_location.y)
		+ patch[1].tex_coord * (1.0f - domain_location.x) * (1.0f - domain_location.y)
		+ patch[2].tex_coord * domain_location.x * domain_location.y
		+ patch[3].tex_coord * (1.0f - domain_location.x) * domain_location.y;

	DS_result result;
	result.position_cs = mul(g_pvm_matrix, float4(pos_ms, 1.0f));
	result.tex_coord = tex_coord;
	result.debug_vec = (float3)path_data.inside_factors[0];
	return result;
}

// ----- Pixel Shader -----

struct PS_result {
	float4 rt_color_0: SV_Target0;
	float4 rt_color_1: SV_Target1;
};

PS_result ps_main(in DS_result frag)
{
	PS_result result;
	result.rt_color_0 = (float4)1.0f;
	result.rt_color_1 = float4(frag.debug_vec, 1.0f);

	return result;
}
