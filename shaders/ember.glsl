// Notes
// 1. we can render only a combination of mesh + material + transform
// 2. users can't define push constants
// 3. vertex format is fixed

#extension GL_EXT_buffer_reference : require
#extension GL_EXT_nonuniform_qualifier : require

#define UNIFORM_BINDING 1
#define TEXTURE_BINDING 2
#define BINDLESS_DESCRIPTOR_SET 0

#define DEF_UBO(Name, Struct) \
 layout(set = BINDLESS_DESCRIPTOR_SET, binding = UNIFORM_BINDING) \
 uniform Name Struct u##Name[]

#define DEF_MATERIAL(Struct) DEF_UBO(Material, Struct)

#define TEXTURE(index, uv) texture(textures[index], uv)

#define MATERIAL (uMaterial[obj.materialIndex])

struct DirectionalLight {
    vec3 direction;     
    vec4 color;     
};

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
	uint materialIndex;
} obj;

layout (set = BINDLESS_DESCRIPTOR_SET, binding = TEXTURE_BINDING) uniform sampler2D textures[];

DEF_UBO(SceneData, {
	mat4 viewproj;
	vec4 ambientColor;
	DirectionalLight sun;
});

#define SCENE uSceneData[0]
