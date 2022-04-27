#version 450

layout (location = 0) out vec4 Color;


layout (push_constant) uniform PushConstants {
	mat2 Transform;
	vec2 Offset;
	vec3 Color;
} pushConstants;


void main() {
	Color = vec4(pushConstants.Color, 1.0);
}
