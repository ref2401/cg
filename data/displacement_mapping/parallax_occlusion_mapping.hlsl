// ----- Vertex shader -----

cbuffer CB_matrices : register(b0) {
	float4x4 g_projection_view_matrix	: packoffset(c0);
	float4x4 g_model_matrix				: packoffset(c4);
	float4x4 g_normal_matrix			: packoffset(c8);
	float3 g_viewpoint_position_ws		: packoffset(c12);
	float g_parallax_step_scale			: packoffset(c12.w);
};

struct Vertex {
	float3 position			: VERT_POSITION_MS;
	float3 normal			: VERT_NORMAL_MS;
	float2 tex_coord		: VERT_TEX_COORD;
	float4 tangent_space	: VERT_TANGENT_SPACE_MS;
};

struct VS_output {
	float4 position_cs					: SV_Position;
	float3 normal_ws					: PIXEL_NORAML_WS;
	float3 dir_to_viewpoint_ws			: PIXEL_DIR_TO_VIEWPOINT_WS;
	float2 tex_coord					: PIXEL_TEX_COORD;
	float2 max_parallax_displacement	: PIXEL_MAX_PARALLAX_DISPLACEMENT;
};

VS_output vs_main(in Vertex vertex) 
{
	// tangent space matrix
	const float3 normal_ws = (mul(g_normal_matrix, float4(vertex.normal, 1.0f))).xyz;
	const float3 tangent_ws = (mul(g_normal_matrix, float4(vertex.tangent_space.xyz, 1.0f))).xyz;
	const float3 bitangent_ws = vertex.tangent_space.w * cross(normal_ws, tangent_ws);
	const float3x3 world_to_tbn_matrix = float3x3(tangent_ws, bitangent_ws, normal_ws);
	// unnormalized direction from viewpoint in tangent space
	const float4 p_ws = mul(g_model_matrix, float4(vertex.position, 1.0f));
	const float3 to_viewpoint_ws = (g_viewpoint_position_ws - p_ws.xyz);
	const float3 to_viewpoint_ts = (mul(world_to_tbn_matrix, to_viewpoint_ws));
	// parallax offset
	const float2 max_parallax_displacement = to_viewpoint_ts.xy / to_viewpoint_ts.z * g_parallax_step_scale;

	VS_output output;
	output.position_cs = mul(g_projection_view_matrix, p_ws);
	output.normal_ws = normal_ws;
	output.tex_coord = vertex.tex_coord;
	output.dir_to_viewpoint_ws = to_viewpoint_ws;
	output.max_parallax_displacement = max_parallax_displacement;
	return output;
}

// ----- Pixel shader -----

cbuffer CB_displacement_mapping : register(b0) {
	float g_sample_min_count	: packoffset(c0.x);
	float g_sample_max_count	: packoffset(c0.y);
};

Texture2D g_tex_diffuse_rgb	: register(t0);
Texture2D g_tex_height_map	: register(t1);
Texture2D g_tex_normal_map	: register(t2);

SamplerState g_sampler : register(s0);

struct PS_output {
	float4 rt_color_0 : SV_Target0;
	float4 rt_color_1 : SV_Target1;
};

float2 calc_parallax_offset(float2 tex_coord, float2 max_parallax_displacement, float step_count)
{
	const float2 dx = ddx(tex_coord);
	const float2 dy = ddy(tex_coord);
	const float step_size = 1.0f / step_count;
	const float2 tc_offset = step_size * max_parallax_displacement;

	float2 tc_curr = tex_coord;
	float ray_height = 1.0f;
	float map_height_curr = g_tex_height_map.SampleGrad(g_sampler, tc_curr, dx, dy).x;
	float map_height_prev = map_height_curr;

	while (ray_height > map_height_curr) {
		map_height_prev = map_height_curr;

		tc_curr -= tc_offset;

		ray_height -= step_size;
		map_height_curr = g_tex_height_map.SampleGrad(g_sampler, tc_curr, dx, dy).x;
	}

	const float denominator = (map_height_prev - map_height_curr - step_size);
	const float weight = (map_height_prev - ray_height - step_size) / denominator;

	return tc_curr + (1.0f - weight) * tc_offset;
}

PS_output ps_main(VS_output pixel)
{
	const float3 normal_ws = normalize(pixel.normal_ws);
	const float3 dir_to_viewpoint_ws = normalize(pixel.dir_to_viewpoint_ws);
	const float cos_nv = dot(normal_ws, dir_to_viewpoint_ws);
	const float step_count = lerp(g_sample_min_count, g_sample_max_count, 1.0f - cos_nv);

	const float2 ptc = calc_parallax_offset(pixel.tex_coord, pixel.max_parallax_displacement, step_count);
	// discard if any component of ptc is out of the range [0 .. 1]
	clip(ptc);
	clip(1.0f - ptc);

	const float4 diffuse_rgb = g_tex_diffuse_rgb.Sample(g_sampler, ptc);

	PS_output output;
	output.rt_color_0 = diffuse_rgb;
	output.rt_color_1 = float4(ptc, 0.0, 1);//pr.debug;
	return output;
}
