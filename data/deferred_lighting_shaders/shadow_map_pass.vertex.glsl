#version 450 core

// assume that GL_MAX_VERTEX_UNIFORM_COMPONENTS = 1024
//
// a mat4 uniform consumes 16 components
// see https://www.opengl.org/wiki/Uniform_(GLSL)#Implementation_limits
//
// u_projection_view_matrix :	16
// u_view_matrix:				16 
// u_arr_model_matrix:			992 (16 * 62)
//					total :		1024
//					unused:		0					
// batch_size for the vertex shader = 62; -> draw_call_index is in [0, 62).


uniform mat4 u_projection_matrix;
uniform mat4 u_view_matrix;
uniform mat4 u_arr_model_matrix[62];

layout(location = 0) in vec3 vert_position;
layout(location = 15) in uint draw_call_index;

out Pixel_data_i {
	float depth_vs;	// direction light's space.
} vs_out;

void main() 
{
	mat4 model_matrix = u_arr_model_matrix[draw_call_index];
	vec4 pos = u_view_matrix * (model_matrix * vec4(vert_position, 1));

	gl_Position = u_projection_matrix * pos;
	vs_out.depth_vs = pos.z;
}
