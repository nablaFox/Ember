#version 450
#extension GL_GOOGLE_include_directive : require

#include "etna.glsl"

layout(location = 0) out vec4 outColor;
layout(location = 1) out vec2 outUV;

void main() {	
    Vertex v = obj.vertexBuffer.vertices[gl_VertexIndex];

    gl_Position = SCENE.viewproj * obj.worldTransform * vec4(v.position, 1.0f);

    outColor = v.color;
	outUV = v.uv;
}
