#version 450 core

// assume that GL_MAX_VERTEX_UNIFORM_COMPONENTS = 1024
//
// a mat3 consumes 12 component, a mat4 uniform consumes 16 comopnents
// see https://www.opengl.org/wiki/Uniform_(GLSL)#Implementation_limits
//
// 16 * 62 + 9 + 16 = 1017 < 1024
// batch_size = 62; -> instance_draw_index in [0, 62).

uniform mat4 u_projection_view_matrix;
uniform mat3 u_view_matrix;				// is use to calculate tangent space basis in the view space
uniform mat4 u_arr_model_matrix[62];

layout(location = 0) in vec3 vert_position;
layout(location = 1) in vec3 vert_normal;
layout(location = 2) in vec2 vert_tex_coord;
layout(location = 3) in vec4 vert_tangent_h;
layout(location = 15) in uint instance_draw_index;

out Pixel_data_i{
	vec3 normal_vs;
	vec3 tangent_vs;
	vec3 bitanget_vs;
	vec2 tex_coord;
	flat uint instance_draw_index;
} vs_out;

void main()
{
	mat4 model_matrix = u_arr_model_matrix[instance_draw_index];
	mat3 normal_matrix_vs = u_view_matrix * mat3(model_matrix); // the assumption is that every scale operation is uniform

	gl_Position = u_projection_view_matrix * (model_matrix * vec4(vert_position, 1));

	vs_out.normal_vs = normal_matrix_vs * vert_normal;
	vs_out.tangent_vs = normal_matrix_vs * vert_tangent_h.xyz;
	vs_out.bitanget_vs = vert_tangent_h.w * cross(vs_out.normal_vs, vs_out.tangent_vs);
	vs_out.tex_coord = vert_tex_coord;
	vs_out.instance_draw_index = instance_draw_index;
}
