#version 330 core

out vec4 frag_color;
in vec2 tex_coords;

uniform sampler2D scene;

uniform bool is_shake;
uniform bool is_confuse;
uniform bool is_chaos;

uniform vec2[9] offsets;
uniform float[9] blur_kernels;
uniform float[9] edge_kernels;

vec4 CalcKernelColor(float kernels[9])
{
	vec3 color = vec3(0.0);

	for(int i = 0; i < 9; ++i) {
		vec3 sample_color = texture(scene, tex_coords.st + offsets[i]).rgb;
		color += sample_color * kernels[i];
	}

	return vec4(color, 1.0);
}

void main() 
{
	vec4 color = texture(scene, tex_coords);

	if(is_shake) {
		color = CalcKernelColor(blur_kernels);
	}

	if(is_chaos) {
		color = CalcKernelColor(edge_kernels);
	}
	else if(is_confuse) {
		color = vec4(1.0 - color.rgb, 1.0);
	}

	frag_color = color;
}