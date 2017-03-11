static const uint compute_kernel_width = 128;
static const uint compute_kernel_height = 1;
static const uint compute_kernel_depth = 1;

cbuffer offset_cbuffer : register(b0) {
	uint2 g_offset_dir : register(c0);
};

Texture2D<float4> g_tex_source	: register(t0);
RWTexture2D<float4> rt_result	: register(u0);


struct Filter_accum {
	float3 color;
	float weight;
};

void process_sample(inout Filter_accum accum, float3 reference, int3 uvw, float kernel_weight);

[numthreads(compute_kernel_width, compute_kernel_height, compute_kernel_depth)]
void cs_main(uint3 dt_id : SV_DispatchThreadID)
{
	const int3 uvw = int3(dt_id);
	const int3 offset = uint3(g_offset_dir, 0);
	const float3 reference = g_tex_source.Load(dt_id).rgb;

	Filter_accum accum = (Filter_accum)0.0f;
	process_sample(accum, reference, (uvw - 3 * offset), 0.030078323f);
	process_sample(accum, reference, (uvw - 2 * offset), 0.104983664f);
	process_sample(accum, reference, (uvw - 1 * offset), 0.222250419f);
	process_sample(accum, reference, (uvw			  ), 0.285375187f);
	process_sample(accum, reference, (uvw + 1 * offset), 0.222250419f);
	process_sample(accum, reference, (uvw + 2 * offset), 0.104983664f);
	process_sample(accum, reference, (uvw + 3 * offset), 0.030078323f);

	rt_result[dt_id.xy] = float4(accum.color / accum.weight, 1.0);
}

void process_sample(inout Filter_accum accum, float3 reference, int3 uvw, float kernel_weight)
{
	static const float sigma = 0.051;
	static const float denominator = 2.0f * sigma * sigma;

	const float3 sample_value = g_tex_source.Load(uvw).rgb;
	const float3 delta = reference - sample_value;
	const float weight = kernel_weight * exp(-1.0f * delta * delta / denominator);

	accum.color += weight * g_tex_source.Load(uvw).rgb;
	accum.weight += weight;
}
