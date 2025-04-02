#version 450
#extension GL_GOOGLE_include_directive : require

#include "etna.glsl"

layout (location = 0) out vec4 outColor;
layout (location = 0) in vec2 uv;

DEF_MATERIAL({
	vec4 color;
	vec4 gridColor;
	float gridSpacing;
	float thickness;
});

void main() {
	float halfThickness = MATERIAL.thickness * 0.5;
	float modX = mod(uv.x + halfThickness, MATERIAL.gridSpacing);
	float modY = mod(uv.y + halfThickness, MATERIAL.gridSpacing);

	if (min(modX, modY) < MATERIAL.thickness) {
		outColor = MATERIAL.gridColor;
		return;
	}

	outColor = MATERIAL.color;
}
