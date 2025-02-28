#version 450
#extension GL_GOOGLE_include_directive : require

layout (location = 0) out vec4 outColor;
layout (location = 0) in vec4 inColor;
layout (location = 1) in vec2 uv;

const float thickness = 0.01;
const vec4 borderColor = vec4(0.0, 0.0, 0.0, 1.0);

void main() {
	if (uv.x < thickness 
		|| uv.x > 1.0 - thickness 
		|| uv.y < thickness 
		|| uv.y > 1.0 - thickness) {
		outColor = borderColor;
		return;
	}

	outColor = inColor;
}

