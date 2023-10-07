#version 330 core
layout (location = 0) in vec4 vertex;

uniform mat4 model_mat;
uniform mat4 proj_mat;

uniform bool is_shake;
uniform bool is_confuse;
uniform bool is_chaos;

uniform float time;

out vec2 tex_coords;

void main()
{
	gl_Position =  proj_mat * model_mat * vec4(vertex.xy, 0.0, 1.0);
	vec2 vertex_tex_coords = vertex.zw;

	if(is_chaos) {
		float strength = 0.3;
		vertex_tex_coords = vec2(vertex_tex_coords.s + sin(time) * strength, vertex_tex_coords.t + cos(time) * strength);
	}
	else if(is_confuse) {
		vertex_tex_coords = vec2(1.0 - vertex_tex_coords.s, 1.0 - vertex_tex_coords.t);
	}

	if(is_shake) {
		float strength = 0.01;
		gl_Position.x += cos(time * 10) * strength;
		gl_Position.y += cos(time * 15) * strength;
	}

	tex_coords = vec2(vertex_tex_coords.s, 1 - vertex_tex_coords.t);
}