#version 450

out VS_result {
	vec2 tex_coord;
} result;

void main()
{
	if (gl_VertexID == 0 || gl_VertexID == 5) {
		gl_Position = vec4(-1, -1, 0, 1);
		result.tex_coord = vec2(0, 0);
	}
	else if (gl_VertexID == 1) {
		gl_Position = vec4(1, -1, 0, 1);
		result.tex_coord = vec2(1, 0);
	}
	else if (gl_VertexID == 2 || gl_VertexID == 3) {
		gl_Position = vec4(1, 1, 0, 1);
		result.tex_coord = vec2(1, 1);
	}
	else if (gl_VertexID == 4) {
		gl_Position = vec4(-1, 1, 0, 1);
		result.tex_coord = vec2(0, 1);
	}
}
