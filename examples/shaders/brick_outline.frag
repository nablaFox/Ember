#version 450
#extension GL_GOOGLE_include_directive : require

layout (location = 0) out vec4 outColor;
layout (location = 0) in vec4 inColor;
layout (location = 1) in vec2 uv;

#include "../../shaders/ember/ember.glsl"

DEF_MATERIAL({
	vec4 borderColor;
	float thickness;
});

void main() {
	if (uv.x < MATERIAL.thickness 
		|| uv.x > 1.0 - MATERIAL.thickness 
		|| uv.y < MATERIAL.thickness 
		|| uv.y > 1.0 - MATERIAL.thickness) {
		outColor = MATERIAL.borderColor;
		return;
	}

	outColor = inColor;
}

