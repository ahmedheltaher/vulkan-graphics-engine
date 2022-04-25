#version 450

layout (location = 0) in vec3 FragColor;
layout (location = 0) out vec4 Color;


void main() {
	Color = vec4(FragColor, 1.0);
}
