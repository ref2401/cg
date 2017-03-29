// ----- Vertex shader -----

cbuffer CB_vertex_shader : register(b0) {
	float4x4 g_projection_view_matrix	: packoffset(c0);
	float4x4 g_model_matrix				: packoffset(c4);
	float4x4 g_normal_matrix			: packoffset(c8);
	float3 g_viewpoint_position_ws		: packoffset(c12);
	float g_parallax_step_scale			: packoffset(c12.w);
	float3 g_light_position_ws			: packoffset(c13);
	float3 g_light_dir_ws				: packoffset(c14);
};

struct Vertex {
	float3 position			: VERT_POSITION_MS;
	float3 normal			: VERT_NORMAL_MS;
	float2 tex_coord		: VERT_TEX_COORD;
	float4 tangent_space	: VERT_TANGENT_SPACE_MS;
};

struct VS_output {
	float4 position_cs					: SV_Position;
	float2 tex_coord					: PIXEL_TEX_COORD;
	float cos_nv						: PIXEL_COS_NV;
	float3 world_up_direction_ts		: PIXEL_WORLD_UP_DIRECTION_TS;
	float2 max_parallax_displacement	: PIXEL_MAX_PARALLAX_DISPLACEMENT;
	float2 light_tex_coord_dir			: PIXEL_LIGHT_TEX_COORD_DIR;
	float3 light_dir_ts					: PIXEL_LIGHT_DIR_TS;
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
	// max parallax offset
	const float2 max_parallax_displacement = to_viewpoint_ts.xy / to_viewpoint_ts.z * g_parallax_step_scale;
	// light tex coord dir
	const float3 to_light_ws = normalize(g_light_position_ws - p_ws.xyz);
	const float2 light_tex_coord_dir = g_parallax_step_scale * mul(world_to_tbn_matrix, to_light_ws).xy;

	VS_output output;
	output.position_cs = mul(g_projection_view_matrix, p_ws);
	output.tex_coord = vertex.tex_coord;
	output.cos_nv = dot(normal_ws, normalize(to_viewpoint_ws));
	output.world_up_direction_ts = mul(world_to_tbn_matrix, float3(0, 1, 0));
	output.max_parallax_displacement = max_parallax_displacement;
	output.light_tex_coord_dir = light_tex_coord_dir;
	output.light_dir_ts = mul(world_to_tbn_matrix, g_light_dir_ws);
	return output;
}

// ----- Pixel shader -----

static const float3 ambient_up_rgb = (float3)0.6f;
static const float3 ambient_down_rgb = (float3)0.4f;

cbuffer CB_displacement_mapping : register(b0) {
	float g_sample_min_count		: packoffset(c0.x);
	float g_sample_max_count		: packoffset(c0.y);
	float g_self_shadowing_factor	: packoffset(c0.z);
};

Texture2D g_tex_diffuse_rgb	: register(t0);
Texture2D g_tex_height_map	: register(t1);
Texture2D g_tex_normal_map	: register(t2);

SamplerState g_sampler : register(s0);

struct PS_output {
	float4 rt_color_0 : SV_Target0;
	float4 rt_color_1 : SV_Target1;
};


// Returns:
//		xy: parallax texture coords which should be used to sample diffuse/normal/whatever
//		z:	height map value at xy coords.
float3 calc_parallax_offset(float2 tex_coord, float2 max_parallax_displacement, float step_count)
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

	return float3(tc_curr + (1.0f - weight) * tc_offset, map_height_curr);
}

float calc_self_shadowing_factor(float2 ptc, float height, float2 tex_coord_dir)
{
	const uint weight_count = 5;
	//const float weights[weight_count] = { 0.25f, 0.5f, 0.75f }; // 3
	const float weights[weight_count] = { 0.18f, 0.36f, 0.54f, 0.72f, 0.9f };
	//const float weights[weight_count] = { 0.125f, 0.25f, 0.375f, 0.5f, 0.625f, 0.75f, 0.875f }; // 7

	float accum = 0.0f;
	[unroll(weight_count)]
	for (uint i = 0; i < weight_count; ++i) {
		const float2 tc = ptc + tex_coord_dir * weights[i];
		const float h = g_self_shadowing_factor * g_tex_height_map.Sample(g_sampler, tc).x;
		accum += weights[i] * (h - height);
	}

	return (1.0f - saturate(accum));
}

PS_output ps_main(VS_output pixel)
{
	const float step_count = lerp(g_sample_min_count, g_sample_max_count, 1.0f - pixel.cos_nv);

	// calculate parallax texture coords offset - ptc.
	// discard if any component of ptc is out of the range [0 .. 1]
	const float3 ptc = calc_parallax_offset(pixel.tex_coord, pixel.max_parallax_displacement, step_count);
	clip(ptc.xy);
	clip(1.0f - ptc.xy);

	const float3 diffuse_rgb = g_tex_diffuse_rgb.Sample(g_sampler, ptc).rgb;
	const float3 normal_ts = g_tex_normal_map.Sample(g_sampler, ptc.xy).rgb * 2.0f - 1.0f;
	const float cos_ti = max(0.0f, dot(normal_ts, -pixel.light_dir_ts));

	const float shadow_factor = calc_self_shadowing_factor(ptc.xy, ptc.z, pixel.light_tex_coord_dir);
	const float alf = dot(normal_ts, pixel.world_up_direction_ts) * 0.5f + 0.5f;
	const float3 ambient_term = diffuse_rgb * lerp(ambient_down_rgb, ambient_up_rgb, alf);
	const float3 diffuse_term = diffuse_rgb * cos_ti / 3.1415f;
	const float3 final_rgb = shadow_factor * (diffuse_term + ambient_term);

	PS_output output;
	output.rt_color_0 = float4(final_rgb, 1.0f);
	output.rt_color_1 = float4(cos_ti, cos_ti, cos_ti, 1.0f);//pr.debug;
	return output;
}
