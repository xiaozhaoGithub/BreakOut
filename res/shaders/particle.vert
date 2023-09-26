#version 330 core
layout (location = 0) in vec4 vertex;

uniform mat4 proj_mat;
uniform vec2 pos;

out vec2 tex_coords;

void main()
{
	float scale = 10.0f;

	gl_Position =  proj_mat * vec4(vertex.xy * scale + pos, 0.0f, 1.0f);
	tex_coords = vec2(vertex.z, vertex.w);
}