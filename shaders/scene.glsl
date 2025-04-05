#extension GL_GOOGLE_include_directive : require

#include "etna.glsl"

DEF_UBO(SceneData, {
	vec4 ambientColor;
	uint lights[];
});

#define SCENE (uSceneData[pc.buff1])

#define CAMERA (uCameraData[pc.buff2])

#define LIGHT(x) (uDirectionalLight[SCENE.lights[x]])

#define AMBIENT (SCENE.ambientColor)
