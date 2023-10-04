#version 330 core

out vec4 frag_color;
in vec2 tex_coords;

uniform sampler2D image;
uniform bool is_shake;

uniform vec2[9] offsets;
uniform float[9] blur_kernels;

void main() 
{
	vec4 color = texture(image, tex_coords);

	if(is_shake) {
		vec4 blur_color = vec4(0.0, 0.0, 0.0, 1.0);
		for(int i = 0; i < 9; ++i) {
			vec4 sample = texture(image, tex_coords.st + offsets[i]);
			sample *= blur_kernels[i];
			blur_color += sample;
		}
		blur_color.a = 1.0;
		color = blur_color;
	}

	frag_color = color;
}