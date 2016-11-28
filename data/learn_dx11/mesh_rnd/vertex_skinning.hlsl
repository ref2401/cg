cbuffer Scene : register(cb0) {
	float4x4 g_projection_view_matrix	: packoffset(c0);
};

cbuffer Model : register(cb1) {
	float4x4 g_model_matrix	: packoffset(c0);
}

StructuredBuffer<float4x4> g_model_bone_matrices : register(t0);

struct Vertex {
	float3 position		: V_POSITION;
	float3 normal		: V_NORMAL;
	float2 tex_coord	: V_TEX_COORD;
	uint4 bone_indices	: V_BONE_INDICES;
	float4 bone_weights	: V_BONE_WEIGHTS;
};

struct VS_output {
	float4 position_cs	: SV_Position;
	float3 position_ws	: FRAG_POSITION_WS;
	float3 normal_ws	: FRAG_NORMAL_WS;
	float3 rgb			: FRAG_RGB;
};

struct Skinned_vertex {
	float3 position;
	float3 normal;
};

Skinned_vertex get_skinned_verte(Vertex vertex);

VS_output vs_main(Vertex vertex)
{
	const Skinned_vertex v = get_skinned_verte(vertex);
	const float3x3 normal_matrix = (float3x3)g_model_matrix;
	const float4 pos_ws = mul(g_model_matrix, float4(v.position, 1));

	VS_output output;
	output.position_ws = pos_ws.xyz;
	output.position_cs = mul(g_projection_view_matrix, pos_ws);
	output.position_cs /= output.position_cs.w;
	output.normal_ws = normalize(mul(normal_matrix, v.normal));
	output.rgb = float3(0.3, 0.3, 0.3);

	return output;
}

Skinned_vertex get_skinned_verte(Vertex vertex)
{
	Skinned_vertex v;
	v.position = float3(0, 0, 0);
	v.normal = float3(0, 0, 0);

	const float4 pos = float4(vertex.position, 1);
	
	// bone #0
	const float bone_weight0 = vertex.bone_weights[0];
	const float4x4 bone_transform_matrix0 = g_model_bone_matrices[vertex.bone_indices[0]];
	const float3x3 bone_normal_matrix0 = (float3x3)bone_transform_matrix0;
	v.position += mul(bone_transform_matrix0, pos).xyz * bone_weight0;
	v.normal += mul(pos, vertex.normal) * bone_weight0;

	// bone #1
	const float bone_weight1 = vertex.bone_weights[1];
	const float4x4 bone_transform_matrix1 = g_model_bone_matrices[vertex.bone_indices[1]];
	const float3x3 bone_normal_matrix1 = (float3x3)bone_transform_matrix1;
	v.position += mul(bone_transform_matrix1, pos).xyz * bone_weight1;
	v.normal += mul(pos, vertex.normal) * bone_weight1;

	// bone #2
	const float bone_weight2 = vertex.bone_weights[2];
	const float4x4 bone_transform_matrix2 = g_model_bone_matrices[vertex.bone_indices[2]];
	const float3x3 bone_normal_matrix2 = (float3x3)bone_transform_matrix2;
	v.position += mul(bone_transform_matrix2, pos).xyz * bone_weight2;
	v.normal += mul(pos, vertex.normal) * bone_weight2;

	// bone #3
	const float bone_weight3 = vertex.bone_weights[3];
	const float4x4 bone_transform_matrix3 = g_model_bone_matrices[vertex.bone_indices[3]];
	const float3x3 bone_normal_matrix3 = (float3x3)bone_transform_matrix3;
	v.position += mul(bone_transform_matrix3, pos).xyz * bone_weight3;
	v.normal += mul(pos, vertex.normal) * bone_weight3;

	return v;
}


static float pi = 3.1415926535;

float3 calc_ambient_term(float3 normal_vs);

float4 ps_main(VS_output frag) : SV_Target
{
	static const float3 light_pos_ws = float3(2, 5, 3);
	
	float3 light_dir_ws = normalize(light_pos_ws - frag.position_ws);
	float cosTi = max(0, dot(frag.normal_ws, light_dir_ws));

	float3 diffuse_rgb = frag.rgb;
	float3 diffuse_term = diffuse_rgb * cosTi / pi;
	float3 ambient_term = diffuse_rgb * calc_ambient_term(frag.normal_ws);
	return float4(diffuse_rgb + ambient_term, 1);
}

float3 calc_ambient_term(float3 normal_vs)
{
	static const float3 down_rgb = float3(0.5, 0.5, 0.5);
	static const float3 up_rgb = float3(1, 1, 1);

	// Hemispheric ambient lighting
	float blend_factor = dot(normal_vs, float3(0, 1, 0)) * 0.5f + 0.5f;
	return 0.5 * lerp(down_rgb, up_rgb, blend_factor);
}