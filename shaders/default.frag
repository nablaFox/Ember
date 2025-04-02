#version 450
#extension GL_GOOGLE_include_directive : require

#include "etna.glsl"

layout (location = 0) out vec4 outColor;

DEF_MATERIAL({
	vec4 color;
});

void main() {
	outColor = MATERIAL.color;
}
