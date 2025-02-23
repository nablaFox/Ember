#version 450
#extension GL_EXT_buffer_reference : require

struct DirectionalLight {
    vec3 direction;     
    vec3 color;     
};

layout(set = 0, binding = 0) uniform SceneData {   
    mat4 viewproj;
    vec4 ambientColor;
    DirectionalLight sun;
} sceneData;

layout(location = 0) out vec3 outColor;

struct Vertex {
    vec3 position;
    vec4 color;
};

layout(buffer_reference, std430) readonly buffer VertexBuffer { 
    Vertex vertices[];
};

layout(push_constant) uniform constants {	
    mat4 worldTransform;
    VertexBuffer vertexBuffer;
} push;

void main() {	
    Vertex v = push.vertexBuffer.vertices[gl_VertexIndex];

    gl_Position = sceneData.viewproj * push.worldTransform * vec4(v.position, 1.0f);

    outColor = v.color.xyz;
}
