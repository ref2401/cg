#version 450 core

// xyz: angular velocity in the model space.
uniform vec3 g_angular_velocity_ms;
// xyz: gravity acceleration in the model space.
// w: simulation delta time. 
uniform vec4 g_external_acceleration_ms;
// x: length
// y: mass
// z: stiffness(coefficient of restitution)
// w: damping coefficient
uniform vec4 g_strand_props;

layout(location = 0) in vec3 vert_p_base;
layout(location = 1) in vec3 vert_p_rest;
layout(location = 2) in vec3 vert_p_curr;
layout(location = 3) in vec3 vert_velocity;

out vec3 tf_p_curr;
out vec3 tf_velocity;
out vec4 tf_debug_slot;

void main()
{
	// spring deformation
	const vec3 deform = vert_p_curr - vert_p_rest;
	// angular acceleration to linear acceleration
	const vec3 r = (g_strand_props.x) * normalize(vert_p_curr - vert_p_base);
	const vec3 linear_velocity_ms = cross(g_angular_velocity_ms, r);

	const vec3 f_total = g_strand_props.y * (g_external_acceleration_ms.xyz)
		- deform * g_strand_props.z
		+ (linear_velocity_ms - vert_velocity) * g_strand_props.w;
	
	const vec3 a = f_total / g_strand_props.y;
	const float t = g_external_acceleration_ms.w;
	const vec3 p_new = vert_p_curr + (vert_velocity * t) + (0.5 * a * t * t);

	const vec3 dir_to_rest = normalize(vert_p_rest - vert_p_base);
	const vec3 dir_to_new_unorm = p_new - vert_p_base;
	
	tf_p_curr = p_new;
	// strand length contstraint
	if (length(dir_to_new_unorm) > g_strand_props.x)
		tf_p_curr = vert_p_base + g_strand_props.x * normalize(dir_to_new_unorm);

	tf_velocity = vert_velocity + a * t;
	tf_debug_slot = vec4(linear_velocity_ms, 24);
}
