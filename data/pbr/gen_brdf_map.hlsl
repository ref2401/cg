// ----- vertex shader -----

static const float3 quad_positions[4] = { 
	float3(-1, -1, 0), float3(1, -1, 0), float3(-1, 1, 0), float3(1, 1, 0)
};

static const float2 quad_uvs[4] = {
	float2(0, 0), float2(1, 0), float2(0, 1), float2(1, 1)
};

struct vs_output {
	float4 position : SV_POSITION;
	float2 uv		: PIXEL_UV;
};

vs_output vs_main(uint vertex_id : SV_VERTEXID)
{
	vs_output o;
	o.position	= float4(quad_positions[vertex_id], 1.0);
	o.uv		= quad_uvs[vertex_id];
	return o;
}

// ----- pixel shader -----

struct ps_output {
	float2 rt_color_0 : SV_TARGET0;
};

static const float pi = 3.1415926535;
static const uint sample_count = 1024;


float2 hammersley(uint i, uint count)
{
	uint bits = count;
	bits = (bits << 16u) | (bits >> 16u);
	bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
	bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
	bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
	bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
	const float van_der_corpus = float(bits) * 2.3283064365386963e-10;
	return float2(float(i) / float(count), van_der_corpus);
}

float3 importance_sample_ggx(float2 xi, float3 n, float roughness)
{
	const float phi = 2.0 * pi * xi.x;
	const float cos_theta = sqrt((1.0 - xi.y) / (1.0 + (roughness * roughness - 1.0) * xi.y));
	const float sin_theta = sqrt(1.0 - cos_theta * cos_theta);

	// from spherical coordinates to cartesian coordinates
	const float3 h_ts = float3(cos(phi) * sin_theta, sin(phi) * sin_theta, cos_theta);
	// from tangent-space vector to world-space sample vector
	const float3 up = abs(n.z) < 0.999 ? float3(0.0, 0.0, 1.0) : float3(1.0, 0.0, 0.0);
	const float3 tangent = normalize(cross(up, n));
	const float3 bitangent = cross(n, tangent);

	return normalize((tangent * h_ts.x) + (bitangent * h_ts.y) + (n * h_ts.z));
}

float masking_shadowing(float3 l_ts, float3 v_ts, float at, float ab, float cos_theta_d)
{
	const float al = ((l_ts.x * l_ts.x * at * at) + (l_ts.y * l_ts.y * ab * ab)) / (l_ts.z * l_ts.z);
	const float av = ((v_ts.x * v_ts.x * at * at) + (v_ts.y * v_ts.y * ab * ab)) / (v_ts.z * v_ts.z);

	const float Al = sqrt(1.0f + al * al) * 0.5f - 0.5f;
	const float Av = sqrt(1.0f + av * av) * 0.5f - 0.5f;

	const float heaviside = (cos_theta_d > 0.0f) ? 1.0f : 0.0f;
	return heaviside / (1 + Al + Av);
}

ps_output ps_main(vs_output pixel)
{
	const float cos_theta_v = pixel.uv.x;
	const float roughness = pixel.uv.y;
	const float3 n_ts = float3(0, 0, 1);
	const float3 v_ts = float3(sqrt(1.0f - cos_theta_v * cos_theta_v), 0.0f, cos_theta_v);

	float a = 0.0f;
	float b = 0.0f;

	for (uint i = 0; i < sample_count; ++i) {
		const float2 xi = hammersley(i, sample_count);
		const float3 h_ts = importance_sample_ggx(xi, n_ts, roughness);
		const float3 l_ts = normalize(2.0 * dot(v_ts, h_ts) * h_ts - v_ts);

		const float cos_theta_l = max(0.0, l_ts.z);
		const float cos_theta_h = max(0.0, h_ts.z);
		const float cos_theta_d = max(0.0, dot(v_ts, h_ts));

		if (cos_theta_l > 0.0)
		{
			const float masking = masking_shadowing(l_ts, v_ts, roughness, roughness, cos_theta_d);
			const float masking_visible = masking * cos_theta_d / (cos_theta_h * cos_theta_v);
			const float fc = pow(1.0 - cos_theta_d, 5.0);

			a += (1.0 - fc) * masking_visible;
			b += fc * masking_visible;
		}
	}

	ps_output o;
	o.rt_color_0 = float2(a / float(sample_count), b / float(sample_count));
	return o;
}
