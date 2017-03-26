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
	float4 position_cs				: SV_Position;
	float3 dir_from_viewpoint_ws	: PIXEL_DIR_FROM_VIEWPOINT_WS;
	float3 normal_ws				: PIXEL_NORMAL_WS;
	float4 tangent_space_ws			: PIXEL_TANGENT_WS;
	float2 tex_coord				: PIXEL_TEX_COORD;
};

VS_output vs_main(in Vertex vertex) 
{
	const float4 p_ws = mul(g_model_matrix, float4(vertex.position, 1.0f));
	const float3 tangent_ws = mul(g_normal_matrix, vertex.tangent_space.xyz);

	VS_output output;
	output.position_cs = mul(g_projection_view_matrix, p_ws);
	output.dir_from_viewpoint_ws = p_ws.xyz - g_viewpoint_position_ws;
	output.normal_ws = mul(g_normal_matrix, float4(vertex.normal, 1.0f)).xyz;
	output.tangent_space_ws = float4(tangent_ws, vertex.tangent_space.w);
	output.tex_coord = vertex.tex_coord;
	return output;
}

// ----- Pixel shader -----

cbuffer CB_displacement_mapping : register(b0) {
	float g_sample_min_count	: packoffset(c0.x);
	float g_sample_max_count	: packoffset(c0.y);
	float g_height_scale		: packoffset(c0.z);
};

Texture2D g_tex_diffuse_rgb : register(t0);
Texture2D g_tex_height_map	: register(t1);
Texture2D g_tex_normal_map	: register(t2);

SamplerState g_sampler {
	Filter = MIN_MAP_MIP_LINEAR;
	AddressU = WRAP;
	AddressV = WRAP;
};

struct PS_output {
	float4 rt_color_0 : SV_Target0;
	float4 rt_color_1 : SV_Target1;
};

struct Parallax_result {
	float2 tex_coord;
	float4 debug;
};

Parallax_result calc_parallax_offset(float2 tex_coord, int step_count, float height_step, float2 uv_step)
{
	Parallax_result result;
	result.tex_coord = tex_coord;
	result.debug = float4(0, 0, 0, 1);
	const float2 dx = ddx(tex_coord);
	const float2 dy = ddy(tex_coord);

	int step_index = 1;
	float prev_ray_height = 1.0f;
	float2 prev_uv_offset = float2(0.0f, 0.0f);
	float prev_map_height = 0.0f;

	while (step_index < step_count) {
		const float curr_ray_height = 1.0f - step_index * height_step;
		const float2 curr_uv_offset = step_index * uv_step;

		const float curr_map_height = g_tex_height_map.SampleGrad(g_sampler,
			tex_coord + curr_uv_offset, dx, dy).x;

		if (curr_ray_height > curr_map_height) {
			prev_uv_offset = curr_uv_offset;
			prev_ray_height = curr_ray_height;
			prev_map_height = curr_map_height;
		}
		else {
			const float denominator = (curr_ray_height - prev_ray_height - curr_map_height + prev_map_height);
			const float factor = (prev_map_height - prev_ray_height) / denominator;
			//result.tex_coord = tex_coord + prev_uv_offset + factor * uv_step;
			result.tex_coord = tex_coord + prev_uv_offset;
			result.debug = float4(tex_coord + curr_uv_offset, curr_uv_offset);
			return result;
			//return float4(curr_ray_height, curr_map_height, float(step_index) / step_count, 1.0f);
		}

		++step_index;
	}

	return result;
	//return float4(0.0f, 0.0f, 0.0f, 1.0f);
}

PS_output ps_main(VS_output pixel)
{
	// tangent space
	const float3 normal_ws = normalize(pixel.normal_ws);
	const float3 tangent_ws = normalize(pixel.tangent_space_ws.xyz);
	const float3 bitangent_ws = pixel.tangent_space_ws.w * cross(normal_ws, tangent_ws);
	const float3x3 to_tbn_matrix = float3x3(tangent_ws, bitangent_ws, normal_ws);
	// sample ray
	const float3 dir_from_viewpoint_ws = pixel.dir_from_viewpoint_ws;
	const float3 dir_from_viewpoint_ts = normalize(mul(to_tbn_matrix, dir_from_viewpoint_ws));
	const float2 max_parallax_offset = -dir_from_viewpoint_ts.xy * g_height_scale;
	// sample step
	const float cos_nv = dot(normal_ws, normalize(-dir_from_viewpoint_ws));
	const int step_count = (int)(lerp(g_sample_min_count, g_sample_max_count, 1.0f - cos_nv));
	const float height_step = 1.0f / (float)step_count;
	const float2 uv_step = height_step * max_parallax_offset;

	
	const Parallax_result parallax_result = calc_parallax_offset(pixel.tex_coord, step_count, height_step, uv_step);
	const float4 diffuse_rgb = g_tex_diffuse_rgb.Sample(g_sampler, parallax_result.tex_coord);

	PS_output output;
	output.rt_color_0 = diffuse_rgb;
	output.rt_color_1 = float4(dir_from_viewpoint_ts, 1.0f);// parallax_result.debug;
	return output;
}
