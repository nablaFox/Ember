#version 450
#extension GL_GOOGLE_include_directive : require

#include "scene.glsl"

layout (location = 0) out vec4 outColor;

struct InstanceData {
	mat4 transform;
	vec4 color;
};

DEF_INSTANCE_DATA(InstanceData);

void main() {	
    gl_Position = CAMERA.viewproj * I.transform * vec4(V.position, 1.0f);

	outColor = I.color;
}
