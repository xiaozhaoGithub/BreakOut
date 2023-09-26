#version 330 core

out vec4 frag_color;
in vec2 tex_coords;

uniform sampler2D image;
uniform vec4 color;

void main() 
{
	vec4 tex_color = texture(image, tex_coords);
	frag_color = tex_color * color;
}