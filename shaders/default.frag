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
layout (location = 0) in vec3 inFragColor;

void main() {
	outFragColor = vec4(inFragColor, 1.0);	
}
