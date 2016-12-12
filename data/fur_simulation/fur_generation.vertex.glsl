#version 450 core

uniform mat4 g_projection_view_matrix;
uniform mat4 g_model_matrix;

layout(location = 0) in vec3 vert_position;
layout(location = 1) in vec3 vert_normal;

out VS_result {
	vec3 normal_ws;
} result;

void main()
{
	mat3 normal_matrix = mat3(g_model_matrix);
	vec4 pos_ws = g_model_matrix * vec4(vert_position, 1);

	gl_Position = g_projection_view_matrix * pos_ws;
	result.normal_ws = normal_matrix * vert_normal;
}
