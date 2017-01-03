#version 450

layout(binding = 0, rgba8) uniform image2D g_img_geometry;
layout(binding = 1, rgba32f) readonly uniform image2D g_img_strand_data;

layout(local_size_x = 1) in;

void main()
{
	ivec2 uv = ivec2(400, 250);
	vec4 strand_data = imageLoad(g_img_strand_data, uv);
	vec2 strand_dir = strand_data.xy;
	float strand_length = strand_data.w;

	imageStore(g_img_geometry, uv, vec4(1, 0, 0, 1));
}
