#version 450
#extension GL_GOOGLE_include_directive : require

#include "scene.glsl"

layout (location = 0) out vec4 outColor;
layout (location = 1) in vec3 inNormal;

DEF_MATERIAL({
    vec4 color;
});

void main() {
    vec3 N = normalize(inNormal);

    outColor = AMBIENT * MATERIAL.color;

    for (uint i = 0; i < SCENE.lightCount; i++) {
        vec3 lightDir = normalize(-LIGHT(i).direction);
        float diff = max(dot(N, lightDir), 0.0);
        outColor += MATERIAL.color * (diff * LIGHT(i).intensity * LIGHT(i).color);
    }
}
