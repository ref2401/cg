#version 450 core

in VS_result {
	vec3 rgb;
	vec3 pos;
	flat int index;
	flat int type;
	flat vec4 debug_slot;
} frag;

layout(location = 0) out vec3 rt_result;

void main()
{
	if (frag.debug_slot.x > frag.debug_slot.y) {
		rt_result = frag.rgb;
	}
	else {
		rt_result = frag.rgb;
	}
}
