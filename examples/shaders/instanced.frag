#version 450
#extension GL_GOOGLE_include_directive : require

#include "etna.glsl"

layout (location = 0) out vec4 outColor;
layout (location = 0) in vec4 inColor;

void main() {
	outColor = inColor;
}
