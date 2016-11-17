struct VS_input {
	float3 position : POSITION;
};

struct VS_output {
	float4 position : SV_Position;
};

VS_output vs_main(VS_input input)
{
	VS_output output;
	output.position = float4(0, 0, 0.2, 1);//float4(input.position, 1);
	return output;
}


float4 ps_main(VS_output input) : SV_Target
{
	return float4(1, 0, 0, 1);
}
