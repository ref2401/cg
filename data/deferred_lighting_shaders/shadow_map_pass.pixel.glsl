#version 450 core

// the shader imposes no restrictions to the batch_size.


in Pixel_data_i {
	float depth_vs; // direction light's space.
} ps_in;

layout(location = 0) out vec2 rt_shadow_map;

void main()
{
	// compute moments M1 and M2 of the depth distribution
	float moment1 = ps_in.depth_vs;
	float moment2 = pow(moment1, 2);

	// partial derivative bias
	float dx = dFdx(ps_in.depth_vs);
	float dy = dFdy(ps_in.depth_vs);
	moment2 += 0.25 * (pow(dx, 2) + pow(dy, 2));

	rt_shadow_map = vec2(moment1, moment2);
}
