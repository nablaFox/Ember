#extension GL_GOOGLE_include_directive : require

#include "etna.glsl"

#define MAX_LIGHTS 16

DEF_UBO(SceneLights, {
	uint lights[MAX_LIGHTS];
});

DEF_UBO(SceneData, {
	vec4 ambientColor;
	uint lightsBuffer;
	uint lightCount;
});

#define SCENE (uSceneData[pc.buff1])

#define CAMERA (uCameraData[pc.buff2])

#define LIGHTS (uSceneLights[SCENE.lightsBuffer])

#define LIGHT(i) (uDirectionalLights[LIGHTS.lights[i]])

#define AMBIENT (SCENE.ambientColor)
