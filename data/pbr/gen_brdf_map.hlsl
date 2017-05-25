// ----- vertex shader -----

static const float3 quad_positions[4] = { 
	float3(-1, -1, 0), float3(1, -1, 0), float3(1, 1, 0), float3(-1, 1, 0)
};

static const float2 quad_uvs[4] = {
	float2(0, 0), float2(1, 0), float2(1, 1), float2(0, 1)
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

ps_output ps_main(vs_output pixel)
{
	ps_output o;
	o.rt_color_0 = pixel.uv;
	return o;
}
