cbuffer Model : register(cb0) {
	float4x4 g_model_matrix	: packoffset(c0);
};

struct VS_result {
	float4 position_ws	: SV_Position;
};

// ----- Vertex Shader -----

VS_result vs_main(uint vertex_id : SV_VertexID)
{
	float4 vertex_position = float4(-1, -1, 0, 1);

	if (vertex_id == 2) {
		vertex_position = float4(1, -1, 0, 1);
	}
	else if (vertex_id == 1) {
		vertex_position = float4(-1, 1, 0, 1);
	}
	
	VS_result result;
	result.position_ws = mul(g_model_matrix, vertex_position);
	
	return result;
}

// ----- Hull Shader -----

struct HS_const_func_result {
	float edges[3]	: SV_TessFactor;
	float inside	: SV_InsideTessFactor;
};

struct HS_result {
	float4 position_ws	: SV_Position;
};

HS_const_func_result hs_const_func_main(InputPatch<VS_result, 3> patch, 
	uint patch_id : SV_PrimitiveID)
{
	static const float factor = 10.0;
	HS_const_func_result result;
	result.edges[0] = factor;
	result.edges[1] = factor;
	result.edges[2] = factor;
	result.inside = factor;

	return result;
}

[domain("tri")]
[partitioning("fractional_even")]
[outputtopology("triangle_ccw")]
[outputcontrolpoints(3)]
[patchconstantfunc("hs_const_func_main")]
HS_result hs_main(InputPatch<VS_result, 3> patch,
	uint control_point_id : SV_OutputControlPointID,
	uint patch_id : SV_PrimitiveID)
{
	HS_result result;
	result.position_ws = patch[control_point_id].position_ws;

	return result;
}

// ----- Domain Shader -----
cbuffer Projection_view : register(cb0) {
	float4x4 g_projection_view_matrix	: packoffset(c0);
};

struct DS_result {
	float4 position_cs : SV_Position;
};

[domain("tri")]
DS_result ds_main(const OutputPatch<HS_result, 3> patch, 
	float3 domain_location : SV_DomainLocation,
	HS_const_func_result cons_func)
{
	const float3 pos_ws = patch[0].position_ws.xyz * domain_location.x
		+ patch[1].position_ws.xyz * domain_location.y
		+ patch[2].position_ws.xyz * domain_location.z;

	DS_result result;
	result.position_cs = mul(g_projection_view_matrix, float4(pos_ws, 1));
	result.position_cs /= result.position_cs.w;
	return result;
}

// ----- Pixel Shader -----

float4 ps_main(VS_result frag) : SV_Target
{
	return float4(1, 0, 0, 1);
}
