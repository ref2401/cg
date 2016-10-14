#version 450 core

// left-bottom, right-bottom, right-top, left-top
uniform vec3 u_arr_far_plane_coords[4];

layout(location = 0) in vec3 vert_position;
layout(location = 2) in vec2 vert_tex_coord;

out Pixel_data_i {
	vec3 view_direction_vs;
} vs_out;

void main()
{
	gl_Position = vec4(vert_position, 1);

	// left-bottom
	if (all(equal(vert_tex_coord, vec2(0, 0))))
		vs_out.view_direction_vs = u_arr_far_plane_coords[0];

	// right-bottom
	if (all(equal(vert_tex_coord, vec2(1, 0))))
		vs_out.view_direction_vs = u_arr_far_plane_coords[1];

	// right-top
	if (all(equal(vert_tex_coord, vec2(1, 1))))
		vs_out.view_direction_vs = u_arr_far_plane_coords[2];

	// left-top
	if (all(equal(vert_tex_coord, vec2(0, 1))))
		vs_out.view_direction_vs = u_arr_far_plane_coords[3];
}
