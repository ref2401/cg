#version 450 core

in VS_result{
	vec3 normal_ws;
} frag;


layout(location = 0) out vec4 rt_color;

const vec3 light_dir_ws = normalize(vec3(0.5, 0.5, 1));

void main()
{
	float cosTi = max(0, dot(light_dir_ws, normalize(frag.normal_ws)));

	rt_color = vec4(cosTi, cosTi, cosTi, 1);
}
