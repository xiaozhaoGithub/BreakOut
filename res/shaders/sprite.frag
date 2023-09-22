#version 330 core

out vec4 frag_color;
in vec2 tex_coords;

uniform sampler2D image;
uniform vec3 sprite_color;

void main() 
{
	vec4 color = texture(image, tex_coords);
	frag_color = vec4(sprite_color, 1.0) * color;
}