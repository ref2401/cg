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

Texture2D g_tex_lookup : register(t0);

cbuffer Tess_control_cbuffer :register(b0) {
	float3 g_camera_position_ms			: packoffset(c0);
	float2 g_distance_min_max			: packoffset(c1);
	float2 g_lod_min_max				: packoffset(c2);
	float g_terrain_grid_z_cell_count	: packoffset(c2.z);
	float g_terrain_grid_x_cell_count	: packoffset(c2.w);
};

struct HS_result_const_func {
	float edge_factors[4]	: SV_TessFactor;
	float inside_factors[2]	: SV_InsideTessFactor;
};

float4 load_lookup_value(uint patch_id, int z_offset, int x_offset)
{
	const int z_cell_count = (int)g_terrain_grid_z_cell_count;
	const int x_cell_count = (int)g_terrain_grid_z_cell_count;

	int patch_z = z_cell_count - patch_id / z_cell_count;
	int patch_x = patch_id % x_cell_count;

	int z_coord = clamp(patch_z + z_offset, 0, z_cell_count - 1);
	int x_coord = clamp(patch_x + x_offset, 0, x_cell_count - 1);

	return g_tex_lookup.Load(uint3(x_coord, z_coord, 0));
}

float3 middle_position(float3 p0, float3 p1, float3 p2, float3 p3)
{
	return 0.25 * (p0 + p1 + p2 + p3);
}

float tess_factor(float3 middle_position_ws, float4 lookup_value)
{
	const float d = distance(g_camera_position_ms, middle_position_ws);
	const float d_normalized = saturate((d - g_distance_min_max.x) / (g_distance_min_max.y - g_distance_min_max.x));
	
	return lerp(g_lod_min_max.x, g_lod_min_max.y, lookup_value.w * pow(1.0f - d_normalized, 1.5));
	//return lerp(g_lod_min_max.x, g_lod_min_max.y,  pow((1.0f - d_normalized), 1.5));
}

HS_result_const_func hs_main_const_func(InputPatch<VS_result, 12> patch, uint patch_id : SV_PrimitiveID)
{
	// patches' middle positions
	const float3 center_ms = middle_position(patch[0].position_ms, patch[1].position_ms, patch[2].position_ms, patch[3].position_ms);
	const float3 left_ms = middle_position(patch[0].position_ms, patch[2].position_ms, patch[4].position_ms, patch[5].position_ms);
	const float3 bottom_ms = middle_position(patch[0].position_ms, patch[1].position_ms, patch[6].position_ms, patch[7].position_ms);
	const float3 right_ms = middle_position(patch[1].position_ms, patch[3].position_ms, patch[8].position_ms, patch[9].position_ms);
	const float3 top_ms = middle_position(patch[2].position_ms, patch[3].position_ms, patch[10].position_ms, patch[11].position_ms);

	// patches' lookup values
	const float4 center_lookup = load_lookup_value(patch_id, 0, 0);
	const float4 left_lookup = load_lookup_value(patch_id, 0, -1);
	const float4 bottom_lookup = load_lookup_value(patch_id, -1, 0);
	const float4 right_lookup = load_lookup_value(patch_id, 0, 1);
	const float4 top_lookup = load_lookup_value(patch_id, 1, 0);

	// patches' lod/tess factors
	const float center_factor = tess_factor(center_ms, center_lookup);
	const float left_factor = tess_factor(left_ms, left_lookup);
	const float bottom_factor = tess_factor(bottom_ms, bottom_lookup);
	const float right_factor = tess_factor(right_ms, right_lookup);
	const float top_factor = tess_factor(top_ms, top_lookup);

	HS_result_const_func result;
	result.edge_factors[0] = min(left_factor, center_factor);
	result.edge_factors[1] = min(bottom_factor, center_factor);
	result.edge_factors[2] = min(right_factor, center_factor);
	result.edge_factors[3] = min(top_factor, center_factor);
	result.inside_factors[0] = center_factor;
	result.inside_factors[1] = center_factor;

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

cbuffer Pvm_matrix_cbuffer : register(b0) {
	float4x4 g_pvm_matrix : packoffset(c0);
};

Texture2D<float> g_tex_displacement_map		: register(t0);
SamplerState g_sampler_default				: register(s0);

struct DS_result {
	float4 position_cs	: SV_Position;
	float2 tex_coord	: FRAG_TEX_COORD;
};

[domain("quad")]
DS_result ds_main(const OutputPatch<HS_result, 4> patch,
	float2 domain_location : SV_DomainLocation,
	HS_result_const_func path_data)
{
	static const float3 normal_ms = float3(0.0, 1.0, 0.0);

	float3 pos_ms = float3(0.0, 0.0, 0.0);
	pos_ms.xz = patch[0].position_ms.xz * domain_location.x * (1.0f - domain_location.y)
		+ patch[1].position_ms.xz * (1.0f - domain_location.x) * (1.0f - domain_location.y)
		+ patch[2].position_ms.xz * domain_location.x * domain_location.y
		+ patch[3].position_ms.xz * (1.0f - domain_location.x) * domain_location.y;

	const float2 tex_coord = patch[0].tex_coord * domain_location.x * (1.0f - domain_location.y)
		+ patch[1].tex_coord * (1.0f - domain_location.x) * (1.0f - domain_location.y)
		+ patch[2].tex_coord * domain_location.x * domain_location.y
		+ patch[3].tex_coord * (1.0f - domain_location.x) * domain_location.y;

	const float displ = 0.5f * g_tex_displacement_map.SampleLevel(g_sampler_default, tex_coord, 0).x;
	pos_ms += displ * normal_ms;

	DS_result result;
	result.position_cs = mul(g_pvm_matrix, float4(pos_ms, 1.0f));
	result.tex_coord = tex_coord;
	return result;
}

// ----- Pixel Shader -----

struct PS_result {
	float4 rt_color_0: SV_Target0;
};

PS_result ps_main(in DS_result frag)
{
	PS_result result;
	result.rt_color_0 = (float4)1.0f;

	return result;
}
