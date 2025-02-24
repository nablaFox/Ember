#version 450
#extension GL_GOOGLE_include_directive : require

#include "ember.glsl"

layout(location = 0) out vec3 outColor;

void main() {	
    Vertex v = obj.vertexBuffer.vertices[gl_VertexIndex];

    gl_Position = SCENE.viewproj * obj.worldTransform * vec4(v.position, 1.0f);

    outColor = v.color.xyz;
}
