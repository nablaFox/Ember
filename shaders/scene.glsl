#extension GL_GOOGLE_include_directive : require

#include "etna.glsl"

struct DirectionalLight {
	vec3 direction;
	vec3 color;
	float intensity;
};

DEF_SSBO(SceneData, {
	vec4 ambientColor;
	DirectionalLight sun;
});

#define SCENE (bSceneData[pc.ssbo])
