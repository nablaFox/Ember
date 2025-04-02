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
	uint sceneData;
	uint cameraData;
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

// Scene data
struct DirectionalLight {
    vec3 direction;     
    vec4 color;     
};

DEF_SSBO(SceneData, {
	vec4 ambientColor;
	DirectionalLight sun;
});

#define SCENE (uSceneData[pc.sceneData])

// Other
DEF_UBO(CameraData, {
	mat4 viewproj;
	mat4 view;
	mat4 proj;
});

#define CAMERA (uCameraData[pc.cameraData])
