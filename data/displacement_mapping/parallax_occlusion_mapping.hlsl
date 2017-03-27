// ----- Vertex shader -----

cbuffer CB_matrices : register(b0) {
	float4x4 g_projection_view_matrix	: packoffset(c0);
	float4x4 g_model_matrix				: packoffset(c4);
	float4x4 g_normal_matrix			: packoffset(c8);
	float3 g_viewpoint_position_ws		: packoffset(c12);
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
	const float g_parallax_step_scale = 0.05f;

	// tangent space matrix
	const float3 normal_ws = (mul(g_normal_matrix, float4(vertex.normal, 1.0f))).xyz;
	const float3 tangent_ws = (mul(g_normal_matrix, float4(vertex.tangent_space.xyz, 1.0f))).xyz;
	const float3 bitangent_ws = vertex.tangent_space.w * cross(normal_ws, tangent_ws);
	const float3x3 world_to_tbn_matrix = float3x3(tangent_ws, bitangent_ws, normal_ws);
	// unnormalized direction from viewpoint in tangent space
	const float4 p_ws = mul(g_model_matrix, float4(vertex.position, 1.0f));
	const float3 from_viewpoint_ws = p_ws.xyz - g_viewpoint_position_ws;
	const float3 from_viewpoint_ts = mul(world_to_tbn_matrix, from_viewpoint_ws);
	// parallax offset
	const float2 parallax_dir = normalize(from_viewpoint_ts.xy);
	const float pl = length(from_viewpoint_ts);
	const float parallax_factor = (pl * pl - pow(from_viewpoint_ts.z, 2)) / from_viewpoint_ts.z;

	VS_output output;
	output.position_cs = mul(g_projection_view_matrix, p_ws);
	output.normal_ws = normal_ws;
	output.tex_coord = vertex.tex_coord;
	output.dir_to_viewpoint_ws = -from_viewpoint_ws;
	output.max_parallax_displacement = parallax_dir * g_parallax_step_scale * parallax_factor;
	return output;
}

// ----- Pixel shader -----

cbuffer CB_displacement_mapping : register(b0) {
	float g_sample_min_count	: packoffset(c0.x);
	float g_sample_max_count	: packoffset(c0.y);
	float g_height_scale		: packoffset(c0.z);
};

Texture2D g_tex_diffuse_rgb	: register(t0);
Texture2D g_tex_height_map	: register(t1);
Texture2D g_tex_normal_map	: register(t2);

SamplerState g_sampler : register(s0);

struct PS_output {
	float4 rt_color_0 : SV_Target0;
	float4 rt_color_1 : SV_Target1;
};

struct Parallax_result {
	float2 tex_coord;
	float4 debug;
};

Parallax_result calc_parallax_offset(float2 tex_coord, float2 max_parallax_displacement, int step_count)
{
	const float2 dx = ddx(tex_coord);
	const float2 dy = ddy(tex_coord);
	const float step_size = 1.0f / step_count;
	const float2 tc_offset = step_size * max_parallax_displacement;

	Parallax_result result;
	result.tex_coord = tex_coord;
	result.debug = float4(0, 0, 0, 1);

	int step_index = 0;
	float prev_ray_height = 1.0f;
	float prev_map_height = 1.0f;

	while (step_index < step_count) {
		const float2 curr_tex_coord = tex_coord + step_index * tc_offset;
		
		const float curr_ray_height = 1.0f - step_index * step_size;
		const float curr_map_height = g_tex_height_map.SampleGrad(g_sampler, curr_tex_coord, dx, dy).x;

		if (curr_ray_height > curr_map_height) {
			prev_map_height = curr_map_height;
			prev_ray_height = curr_ray_height;
		}
		else {
			/*const float2 pt1 = float2(curr_ray_height, curr_map_height);
			const float2 pt2 = float2(curr_ray_height + step_size, prev_map_height);
			const float fDelta2 = pt2.x - pt2.y;
			const float fDelta1 = pt1.x - pt1.y;
			const float fDenominator = fDelta2 - fDelta1;
			float fParallaxAmount = 0.0f;
			if (fDenominator != 0.0f)
			{
				fParallaxAmount = (pt1.x * fDelta2 - pt2.x * fDelta1) / fDenominator;
			}
			result.tex_coord = tex_coord + max_parallax_displacement * (1.0f - fParallaxAmount);*/

			result.tex_coord = curr_tex_coord;
			result.debug = float4(curr_tex_coord - tex_coord, tc_offset);
			return result;
		}

		++step_index;
	}

	return result;
}

PS_output ps_main(VS_output pixel)
{
	const float3 normal_ws = normalize(pixel.normal_ws);
	const float3 dir_to_viewpoint_ws = normalize(pixel.dir_to_viewpoint_ws);
	const float cos_nv = dot(normal_ws, dir_to_viewpoint_ws);
	const float step_count = lerp(g_sample_min_count, g_sample_max_count, 1.0f - cos_nv);

	const Parallax_result pr = calc_parallax_offset(pixel.tex_coord, pixel.max_parallax_displacement, (int)step_count);
	const float4 diffuse_rgb = g_tex_diffuse_rgb.Sample(g_sampler, pr.tex_coord);

	PS_output output;
	output.rt_color_0 = diffuse_rgb;
	output.rt_color_1 = pr.debug;
	return output;
}
