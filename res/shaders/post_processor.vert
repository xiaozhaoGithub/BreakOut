#version 330 core
layout (location = 0) in vec4 vertex;

uniform mat4 model_mat;
uniform mat4 proj_mat;
uniform bool is_shake;
uniform float time;

out vec2 tex_coords;

void main()
{
	gl_Position =  proj_mat * model_mat * vec4(vertex.xy, 0.0, 1.0);
	tex_coords = vec2(vertex.z, 1 - vertex.w);

	if(is_shake) {
		float strength = 0.01;
		gl_Position.x += cos(time * 10) * strength;
		gl_Position.y += cos(time * 15) * strength;
	}
	
}