static const uint kernel_width = 16;
static const uint kernel_height = 16;

Texture2D g_tex_displacement_map	: register(t0);

struct Dispatch_desc {
	uint3 group_id		: SV_GroupID;
	uint3 dt_id			: SV_DispatchThreadID;
	uint3 gt_id			: SV_GroupThreadID;
	uint group_index	: SV_GroupIndex;
};

RWTexture2D<float4>	out_tex_patch_plane;


groupshared float kernel[kernel_width * kernel_height];
groupshared float4 plane;
groupshared float3 raw_normals[2][2];
groupshared float3 corners[2][2];

[numthreads(kernel_width, kernel_height, 1)]
void cs_main(Dispatch_desc desc)
{
	const bool is_left_top = all(desc.dt_id == uint3(0, 0, 0));
	const bool is_left_bottom = all(desc.dt_id == uint3(0, kernel_height - 1, 0));
	const bool is_right_top = all(desc.dt_id == uint3(kernel_width - 1, 0, 0));
	const bool is_right_bottom = all(desc.dt_id == uint3(kernel_width - 1, kernel_height - 1, 0));

	if (is_left_top || is_left_bottom || is_right_top || is_right_bottom) {
		const float displ = g_tex_displacement_map.Load(uint3(desc.dt_id.xy, 0)).x;
		kernel[desc.group_index] = displ;
		
		const uint corner_x = desc.gt_id.x / (kernel_width - 1);
		const uint corner_y = desc.gt_id.y / (kernel_height - 1);
		corners[corner_x][corner_y] = float3(corner_x, displ, corner_y);
	}

	GroupMemoryBarrierWithGroupSync();

	if (is_left_top) {
		raw_normals[0][0] = normalize(cross(
			corners[0][1] - corners[0][0], 
			corners[1][0] - corners[0][0])
		);
	}
	else if (is_left_bottom) {
		raw_normals[0][1] = normalize(cross(
			corners[1][1] - corners[0][1],
			corners[0][0] - corners[0][1])
		);
	}
	else if (is_right_top) {
		raw_normals[1][0] = normalize(cross(
			corners[0][0] - corners[1][0],
			corners[1][1] - corners[1][0])
		);
	}
	else if (is_right_bottom) {
		raw_normals[1][1] = normalize(cross(
			corners[1][0] - corners[1][1],
			corners[0][1] - corners[1][1])
		);
	}
	else {
		kernel[desc.group_index] = g_tex_displacement_map.Load(uint3(desc.dt_id.xy, 0)).x;
	}

	GroupMemoryBarrierWithGroupSync();

	if (desc.group_index == 0) {
		const float3 normal = normalize(raw_normals[0][0]
			+ raw_normals[0][1]
			+ raw_normals[1][0]
			+ raw_normals[1][1]);

		float3 min_corner = float3(0.0f, 1e9f, 0.0f);
		if (min_corner.y < corners[0][0].y) min_corner = corners[0][0];
		if (min_corner.y < corners[0][1].y) min_corner = corners[0][1];
		if (min_corner.y < corners[1][0].y) min_corner = corners[1][0];
		if (min_corner.y < corners[1][1].y) min_corner = corners[1][1];

		plane = float4(normal, dot(normal, min_corner));
	}

	GroupMemoryBarrierWithGroupSync();

	const float3 position = float3(desc.gt_id.x / (float)kernel_width, 
		kernel[desc.group_index],
		desc.gt_id.y / (float)kernel_height);
	kernel[desc.group_index] = dot(plane.xyz, position) - plane.w;

	GroupMemoryBarrierWithGroupSync();

	if (desc.group_index == 0) {
		float dev = 0.0f;

		for (uint i = 0; i < kernel_width * kernel_height; ++i)
			dev += pow(kernel[i], 2);

		dev /= (float)(kernel_width * kernel_height - 1);
		dev = sqrt(dev);
		out_tex_patch_plane[desc.group_id.xy] = float4(plane.xyz, dev);
	}
}
