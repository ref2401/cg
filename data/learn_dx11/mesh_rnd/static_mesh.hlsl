cbuffer Scene : register(cb0) {
	float4x4 g_projection_matrix	: packoffset(c0);
	float4x4 g_view_matrix			: packoffset(c4);
};

cbuffer Model : register(cb1) {
	float4x4 g_model_matrix			: packoffset(c0);
};

struct Vertex {
	float3 position		: V_POSITION;
	float3 normal		: V_NORMAL;
	float2 tex_coord	: V_TEX_COORD;
};

struct VS_output {
	float4 position		: SV_Position;
	float3 normal_ws	: NORMAL_WS;
	float2 tex_coord	: TEX_COORD;
};

VS_output vs_main(Vertex vertex)
{
	float4 pos_ws = mul(g_model_matrix, float4(vertex.position, 1));
	float4 pos_cs = mul(mul(g_projection_matrix, g_view_matrix), pos_ws);

	VS_output output;
	output.position = pos_cs;
	output.normal_ws = mul(g_model_matrix, float4(vertex.normal, 1)).xyz;
	output.tex_coord = vertex.tex_coord;
	return output;
}


static float pi = 3.1415926535;
SamplerState sampler_linear			: register(s0);
Texture2D<float3> g_tex_diffuse_rgb : register(t0);

float3 calc_ambient_term(float3 normal_vs);

float4 ps_main(VS_output frag) : SV_Target
{
	static const float3 direction_to_light = normalize(float3(2, 5, 3));
	float cosTi = max(0, dot(frag.normal_ws, direction_to_light));
	
	float3 diffuse_rgb = g_tex_diffuse_rgb.Sample(sampler_linear, frag.tex_coord);
	float3 diffuse_term = diffuse_rgb * cosTi / pi;
	float3 ambient_term = diffuse_rgb * calc_ambient_term(frag.normal_ws);

	return float4(ambient_term + diffuse_term, 1);
}

float3 calc_ambient_term(float3 normal_vs)
{
	static const float3 down_rgb = float3(0.5, 0.5, 0.5);
	static const float3 up_rgb = float3(1, 1, 1);

	// Hemispheric ambient lighting
	float blend_factor = dot(normal_vs, float3(0, 1, 0)) * 0.5f + 0.5f;
	return 0.5 * lerp(down_rgb, up_rgb, blend_factor);
}
