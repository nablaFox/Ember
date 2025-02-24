#version 450
#extension GL_GOOGLE_include_directive : require

#include "ember.glsl"

layout (location = 0) out vec4 outFragColor;
layout (location = 0) in vec3 inFragColor;

void main() {
	outFragColor = vec4(inFragColor, 1.0);	
}
