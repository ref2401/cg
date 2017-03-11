static const uint compute_kernel_width = 128;
static const uint compute_kernel_height = 1;
static const uint compute_kernel_depth = 1;

cbuffer offset_cbuffer : register(b0) {
	uint2 g_offset_dir : register(c0);
};

Texture2D<float4> g_tex_source	: register(t0);
RWTexture2D<float4> rt_result	: register(u0);


[numthreads(compute_kernel_width, compute_kernel_height, compute_kernel_depth)]
void cs_main(uint3 dt_id : SV_DispatchThreadID)
{
	const int3 uvw = int3(dt_id);
	const int3 offset = uint3(g_offset_dir, 0);
	float3 accum = (float3)0.0f
		+ 0.030078323f * g_tex_source.Load(uvw - 3 * offset).rgb
		+ 0.104983664f * g_tex_source.Load(uvw - 2 * offset).rgb
		+ 0.222250419f * g_tex_source.Load(uvw - 1 * offset).rgb
		+ 0.285375187f * g_tex_source.Load(uvw).rgb
		+ 0.222250419f * g_tex_source.Load(uvw + 1 * offset).rgb
		+ 0.104983664f * g_tex_source.Load(uvw + 2 * offset).rgb
		+ 0.030078323f * g_tex_source.Load(uvw + 3 * offset).rgb;

	rt_result[dt_id.xy] = float4(accum, 1.0);
}
