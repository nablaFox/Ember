#version 450
#extension GL_GOOGLE_include_directive : require

#include "etna.glsl"

layout(location = 0) out vec4 outColor;
layout(location = 1) out vec2 outUV;

void main() {	
    gl_Position = SCENE.viewproj * pc.worldTransform * vec4(V.position, 1.0f);

	outColor = V.color;
	outUV = V.uv;
}
