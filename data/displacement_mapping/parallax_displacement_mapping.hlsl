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
	float4 position_cs			: SV_Position;
	float3 dir_to_viewpoint_ws	: PIXEL_POSITION_WS;
	float3 normal_ws			: PIXEL_NORMAL_WS;
	float4 tangent_space_ws		: PIXEL_TANGENT_WS;
	float2 tex_coord			: PIXEL_TEX_COORD;
};

VS_output vs_main(in Vertex vertex) 
{
	const float4 p_ws = mul(g_model_matrix, float4(vertex.position, 1.0f));
	const float3 tangent_ws = mul(g_model_matrix, float4(vertex.tangent_space.xyz, 1.0f)).xyz;

	VS_output output;
	output.position_cs = mul(g_projection_view_matrix, p_ws);
	output.dir_to_viewpoint_ws = g_viewpoint_position_ws - p_ws.xyz;
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
};

PS_output ps_main(VS_output pixel)
{
	const float2 dx = ddx(pixel.tex_coord);
	const float2 dy = ddy(pixel.tex_coord);
	const float3 normal_ws = normalize(pixel.normal_ws);
	const float3 dir_to_view_ws = normalize(pixel.dir_to_viewpoint_ws);
	const float3 tangent_ws = normalize(pixel.tangent_space_ws.xyz);
	const float3 bitangent_ws = pixel.tangent_space_ws.w * cross(normal_ws, tangent_ws);
	const float3x3 to_tbn_matrix = float3x3(tangent_ws, bitangent_ws, normal_ws);
	const float3 dir_to_viewpoint_ts = mul(to_tbn_matrix, dir_to_view_ws);
	
	const float2 max_parallax_offset = -dir_to_viewpoint_ts.yx * g_height_scale / dir_to_viewpoint_ts.z;
	const int step_count = (int)lerp(g_sample_min_count, g_sample_max_count, dot(normal_ws, dir_to_view_ws));
	const float height_step = 1.0f / (float)step_count;
	const float2 uv_step = height_step * max_parallax_offset;

	int step_index = 0;
	float curr_ray_height = 1.0f - height_step;
	float prev_ray_height = 1.0f;
	float2 curr_uv_offset = 0;
	float2 prev_uv_offset = 0;
	float prev_map_height = 0.0f;
	float curr_map_height = 0.0f;
	float2 parallax_tex_coord = pixel.tex_coord;

	while (step_index < step_count) {
		curr_map_height = g_tex_height_map.SampleGrad(g_sampler,
			pixel.tex_coord + curr_uv_offset, dx, dy).x;

		if (curr_ray_height <= curr_map_height) {
			prev_uv_offset = curr_uv_offset;
			prev_ray_height = curr_ray_height;
			prev_map_height = curr_map_height;

			curr_uv_offset += uv_step;
			curr_ray_height -= height_step; // going deeper into the surface
		}
		else {
			const float factor = (prev_map_height - prev_ray_height) 
				/ (curr_ray_height - prev_ray_height - curr_map_height + prev_map_height);

			parallax_tex_coord = pixel.tex_coord + prev_uv_offset + factor * uv_step;

			break;
		}

		++step_index;
	}


	const float4 diffuse_rgb = g_tex_diffuse_rgb.Sample(g_sampler, parallax_tex_coord);

	PS_output output;
	output.rt_color_0 = diffuse_rgb;
	return output;
}
