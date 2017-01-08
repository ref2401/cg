#version 450 core

in VS_result {
	vec3 rgb;
	vec3 pos;
	flat int index;
	flat int type;
} frag;

layout(location = 0) out vec3 rt_result;

void main()
{
	rt_result = frag.rgb;
}
