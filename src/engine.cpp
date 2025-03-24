#include "engine.hpp"
#include "primitives.hpp"
#include <GLFW/glfw3.h>

using namespace etna;

Engine::Engine() {
	glfwInit();

	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions =
		glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char*> extensions(glfwExtensions,
										glfwExtensions + glfwExtensionCount);

	m_device = new ignis::Device({
		.shadersFolder = "shaders",
		.extensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME},
		.instanceExtensions = extensions,
		.optionalFeatures = {"FillModeNonSolid", "SampleRateShading"},
	});

	// create default material
	m_defaultMaterial =
		new _Material(this, {.shaders = {"default.vert.spv", "default.frag.spv"}});

	std::vector<Vertex> primitiveVertices;
	std::vector<Index> primitiveIndices;

	utils::createTexturedSphere(primitiveVertices, primitiveIndices);
	utils::createTexturedCube(primitiveVertices, primitiveIndices);

	m_primitiveVertices = m_device->createSSBO(
		sizeof(Vertex) * primitiveVertices.size(), primitiveVertices.data());

	m_primitiveIndices = m_device->createSSBO(
		sizeof(Index) * primitiveIndices.size(), primitiveIndices.data());

	m_graphicsQueue = 0;
	m_uploadQueue = 0;
}

Engine::~Engine() {
	delete m_device;
	delete m_defaultMaterial;

	m_device->destroyBuffer(m_primitiveVertices);
	m_device->destroyBuffer(m_primitiveIndices);

	glfwTerminate();
}

Mesh Engine::createSphere() {
	_Mesh::CreateInfo2 info{
		.vertexBuffer = m_primitiveVertices,
		.indexBuffer = m_primitiveIndices,
		.firstIndex = 0,
		.indexCount = utils::SPHERE_PRIMITIVE_INDICES_COUNT,
	};

	return std::shared_ptr<_Mesh>(new _Mesh(info));
}

Mesh Engine::createCube() {
	_Mesh::CreateInfo2 info{
		.vertexBuffer = m_primitiveVertices,
		.indexBuffer = m_primitiveIndices,
		.firstIndex = utils::SPHERE_PRIMITIVE_INDICES_COUNT,
		.indexCount = utils::CUBE_PRIMITIVE_INDICES_COUNT,
	};

	return std::shared_ptr<_Mesh>(new _Mesh(info));
}
