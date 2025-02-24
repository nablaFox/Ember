#version 450
#extension GL_GOOGLE_include_directive : require

#include "ember.glsl"

// DEF_MATERIAL({
// 	vec3 test;
// 	int albedo;
// });

layout (location = 0) out vec4 outFragColor;
layout (location = 0) in vec3 inFragColor;

void main() {	
	// vec3 test = MATERIAL.test;
	// vec3 albedo = TEXTURE(MATERIAL.albedo, vec2(0.0, 0.0)).rgb;

	outFragColor = vec4(inFragColor, 1.0);	
}
