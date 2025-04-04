#extension GL_GOOGLE_include_directive : require

#include "etna.glsl"

DEF_UBO(CameraData, {
	mat4 viewproj;
	mat4 view;
	mat4 proj;
});

#define CAMERA (uCameraData[pc.ubo])
