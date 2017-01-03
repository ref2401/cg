#version 450

layout(binding = 0) uniform sampler2D tex_diffuse_rgb;


layout(location = 0) out vec3 rt_geometry;
layout(location = 1) out vec4 rt_strand_data;

in VS_result{
	vec4 strand_p0_cs;
	vec4 strand_p1_cs;
	vec2 tex_coord;
} frag;

void main()
{
	vec3 diffuse_rgb = texture(tex_diffuse_rgb, frag.tex_coord).rgb;

	vec3 strand_p0_ndc = frag.strand_p0_cs.xyz / frag.strand_p0_cs.w;
	vec3 strand_p1_ndc = frag.strand_p1_cs.xyz / frag.strand_p1_cs.w;
	vec3 diff_ndc = strand_p1_ndc - strand_p0_ndc;
	vec3 dir_ndc = normalize(diff_ndc);
	float length_ndc = length(diff_ndc);

	rt_geometry = diffuse_rgb;
	rt_strand_data = vec4(dir_ndc.xy, length_ndc, 1);
}
