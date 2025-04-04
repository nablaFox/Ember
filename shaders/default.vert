#version 450
#extension GL_GOOGLE_include_directive : require

#include "camera.glsl"

layout(location = 0) out vec2 outUV;

void main() {	
    gl_Position = CAMERA.viewproj * pc.worldTransform * vec4(V.position, 1.0f);
	outUV = V.uv;
}
