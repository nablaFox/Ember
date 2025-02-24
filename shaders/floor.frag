#version 450
#extension GL_GOOGLE_include_directive : require

#include "ember.glsl"

DEF_MATERIAL({
	vec3 testColor;
});

layout (location = 0) out vec4 outFragColor;
layout (location = 0) in vec3 inFragColor;

void main() {	
	outFragColor = vec4(MATERIAL.testColor, 1.0);	
}
