#version 450

layout (location = 0) in vec2 Position;
layout (location = 1) in vec3 Color;

layout (push_constant) uniform PushConstants {
	mat2 Transform;
	vec2 Offset;
	vec3 Color;
} pushConstants;

void main() {
	gl_Position = vec4(pushConstants.Transform * Position + pushConstants.Offset, 0.0, 1.0);
}
