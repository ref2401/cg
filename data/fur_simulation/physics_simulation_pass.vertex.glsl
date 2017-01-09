#version 450 core

// xyz: gravity acceleration in model space
// w: simulation delta time. 
uniform vec4 g_gravity_ms;
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


void main()
{
	const vec3 deform = vert_p_curr - vert_p_rest;
	const vec3 f_total = g_strand_props.y * g_gravity_ms.xyz
		- deform * g_strand_props.z
		- vert_velocity * g_strand_props.w;
	
	const vec3 a = f_total / g_strand_props.y;
	const float t = g_gravity_ms.w;
	const vec3 p_new = vert_p_curr + (vert_velocity * t) + (0.5 * a * t * t);

	tf_p_curr = vert_p_base + g_strand_props.x * normalize(p_new - vert_p_base); // strand length preservation
	tf_velocity = vert_velocity + a * t;

	//tf_p_curr = p_new - vert_p_base;
}
