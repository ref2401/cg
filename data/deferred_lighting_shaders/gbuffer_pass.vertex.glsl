#version 450 core

// assume that GL_MAX_VERTEX_UNIFORM_COMPONENTS = 1024
//
// a mat4 uniform consumes 16 components
// see https://www.opengl.org/wiki/Uniform_(GLSL)#Implementation_limits
//
// u_projection_matrix:			16
// u_view_matrix:				16 
// u_arr_model_matrix:			992 (16 * 62)
//					total :		1024
//					unused:		0					
// batch_size for the vertex shader = 62; -> draw_call_index is in [0, 62).
// batch_size for the pixel shader = 32 (see gbuffer_pass.pixel.glsl)
const uint batch_size = 32;

uniform	mat4 u_projection_matrix;
uniform mat4 u_view_matrix;
uniform mat4 u_arr_model_matrix[batch_size];

layout(location = 0) in vec3 vert_position;
layout(location = 1) in vec3 vert_normal;
layout(location = 2) in vec2 vert_tex_coord;
layout(location = 3) in vec4 vert_tangent_h;
layout(location = 15) in uint draw_call_index;

out Pixel_data_i {
	vec3 normal_vs;
	vec3 tangent_vs;
	vec3 bitanget_vs;
	vec2 tex_coord;
	float depth_vs;
	flat uint draw_call_index;
} vs_out;


void main()
{
	mat4 model_matrix = u_arr_model_matrix[draw_call_index];

	vec4 posistion_vs = u_view_matrix * (model_matrix * vec4(vert_position, 1));
	gl_Position = u_projection_matrix * posistion_vs;

	mat3 normal_matrix_vs = mat3(u_view_matrix) * mat3(model_matrix); // the assumption is that every scale operation is uniform
	vs_out.normal_vs = normal_matrix_vs * vert_normal;
	vs_out.tangent_vs = normal_matrix_vs * vert_tangent_h.xyz;
	vs_out.bitanget_vs = vert_tangent_h.w * cross(vs_out.normal_vs, vs_out.tangent_vs);
	vs_out.tex_coord = vert_tex_coord;
	vs_out.depth_vs = posistion_vs.z;
	vs_out.draw_call_index = draw_call_index;
}