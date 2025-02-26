#version 450
#extension GL_GOOGLE_include_directive : require

#include "ember.glsl"

layout (location = 0) out vec4 outColor;
layout (location = 0) in vec4 inColor;
layout (location = 1) in vec2 uv;

DEF_MATERIAL({
	vec4 color;
	float lines;
	float thickness;
});

void main() {
	float halfThickness = MATERIAL.thickness * 0.5;
	float gridSpacing = 1.0 / MATERIAL.lines;
	float modX = mod(uv.x + halfThickness, gridSpacing);
	float modY = mod(uv.y + halfThickness, gridSpacing);

	if (min(modX, modY) < MATERIAL.thickness) {
		outColor = MATERIAL.color;
		return;
	}

	outColor = inColor;
}
