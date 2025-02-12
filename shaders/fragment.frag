#version 450

struct DirectionalLight {
    vec3 direction;     
    vec3 color;     
};

layout(set = 0, binding = 0) uniform SceneData {   
    mat4 viewproj;
    vec4 ambientColor;
    DirectionalLight sun;
} sceneData;

layout (location = 0) out vec4 outFragColor;

void main() {
	outFragColor = vec4(sceneData.ambientColor.rgb, 1.0);
}
