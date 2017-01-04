#version 450

layout(binding = 0, rgba32f) uniform image2D g_img_geometry;
layout(binding = 1) uniform sampler2D g_img_strand_data;

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

void main()
{
	const ivec2 p0 = ivec2(gl_WorkGroupID.xy);
	const vec4 diffuse_rgb = imageLoad(g_img_geometry, p0);
	const vec4 strand_data = texelFetch(g_img_strand_data, p0, 0);
	const ivec2 p1 = ivec2(strand_data.xy * vec2(gl_NumWorkGroups.xy));
	const uint strand_length = 10;//uint(ceil(strand_data.z));

	for (uint i = 0; i < strand_length; ++i) {
		for (uint j = 0; j < strand_length; ++j) {
			imageStore(g_img_geometry, p0 + ivec2(i, j), diffuse_rgb);
		}
	}
}
