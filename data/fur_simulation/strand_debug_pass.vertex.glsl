#version 450 core

					uniform mat4			g_pvm_matrix;
layout(binding = 0) uniform samplerBuffer	tbo_p_base;
layout(binding = 1) uniform samplerBuffer	tbo_p_curr;
layout(binding = 2) uniform samplerBuffer	tbo_debug_slot;


out VS_result {
	vec3 rgb;
	vec3 pos;
	flat int index;
	flat int type;
	flat vec4 debug_slot;
} result;

void main()
{
	const int vertex_index = gl_VertexID / 4;
	const int vertex_type = gl_VertexID % 4;
	
	// TODO(ref2401): optimize branches
	vec4 p;
	if (vertex_type == 0) {
		p = texelFetch(tbo_p_base, vertex_index * 2);
		result.rgb = vec3(1, 0, 1);
	}
	else if (vertex_type == 1) {
		p = texelFetch(tbo_p_base, vertex_index * 2 + 1);
		result.rgb = vec3(1, 0, 1);
	}
	else if (vertex_type == 2) {
		p = texelFetch(tbo_p_base, vertex_index * 2);
		result.rgb = vec3(0, 1, 1);
	}
	else if (vertex_type == 3) {
		p = texelFetch(tbo_p_curr, vertex_index * 2);
		result.rgb = vec3(0, 1, 1);
	}

	gl_Position = g_pvm_matrix * p;
	
	result.pos = p.xyz;
	result.index = vertex_index;
	result.type = vertex_type;
	result.debug_slot = texelFetch(tbo_debug_slot, vertex_index);
}
