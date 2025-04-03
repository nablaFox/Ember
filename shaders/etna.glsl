#extension GL_EXT_buffer_reference : require
#extension GL_EXT_nonuniform_qualifier : require

#define STORAGE_BUFFER_BINDING 0
#define UNIFORM_BINDING 1

#define DEF_UBO(Name, Struct) \
 layout(set = 0, binding = UNIFORM_BINDING) \
 uniform Name Struct u##Name[]

#define DEF_SSBO(Name, Struct) \
 layout(set = 0, binding = STORAGE_BUFFER_BINDING) \
 readonly buffer Name Struct u##Name[] 

layout(push_constant) uniform constants {
	mat4 worldTransform;
	uint verticesIndex;
	uint materialIndex;
	uint instanceBufferIndex;
	uint ubo;
	uint ssbo;
} pc;

// Vertices
struct Vertex {
    vec3 position;
	vec2 uv;
};

DEF_SSBO(VertexBuffer, {
	Vertex vertices[];
});

#define V (uVertexBuffer[pc.verticesIndex].vertices[gl_VertexIndex])

// Material
#define DEF_MATERIAL(Struct) DEF_UBO(Material, Struct)

#define MATERIAL (uMaterial[pc.materialIndex])
