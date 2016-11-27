cbuffer Scene : register(cb0) {
	float4x4 g_projection_view_matrix	: packoffset(c0);
};

cbuffer Model : register(cb1) {
	float4x4 g_model_matrix	: packoffset(c0);
}

StructuredBuffer<float4x4> g_model_bone_matrices : register(t0);

struct Vertex {
	uint id				: SV_VERTEXID;
	float3 position		: V_POSITION;
	float3 normal		: V_NORMAL;
	float2 tex_coord	: V_TEX_COORD;
};

struct VS_output {
	float4 position_cs	: SV_Position;
	float3 position_ws	: FRAG_POSITION_WS;
	float3 normal_ws	: FRAG_NORMAL_WS;
	float3 rgb			: FRAG_RGB;
};


VS_output vs_main(Vertex vertex)
{
	//float4x4 bone_matrix = g_model_bone_matrices[vertex_id];
	//float4 pos = float4(bone_matrix._14_24_34, 1);
	const float3x3 normal_matrix = (float3x3)g_model_matrix;
	float4 pos_ws = mul(g_model_matrix, float4(vertex.position, 1));

	VS_output output;
	output.position_ws = pos_ws;
	output.position_cs = mul(g_projection_view_matrix, pos_ws);
	output.position_cs /= output.position_cs.w;
	output.normal_ws = normalize(mul(normal_matrix, float4(vertex.normal, 1)));
	output.rgb = float3(0.3, 0.3, 0.3);

	return output;
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