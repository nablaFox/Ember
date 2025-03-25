#include "engine.hpp"
#include "primitives.hpp"
#include <GLFW/glfw3.h>

using namespace etna;

ignis::Device* g_device = nullptr;

ignis::Device& Engine::getDevice() {
	if (g_device != nullptr)
		return *g_device;

	glfwInit();

	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions =
		glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char*> extensions(glfwExtensions,
										glfwExtensions + glfwExtensionCount);

	g_device = new ignis::Device({
		.shadersFolder = "shaders",
		.extensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME},
		.instanceExtensions = extensions,
		.optionalFeatures = {"FillModeNonSolid", "SampleRateShading"},
	});

	return *g_device;
}

Engine::Engine() {
	m_defaultMaterial =
		new _Material({.shaders = {"default.vert.spv", "default.frag.spv"}});

	m_graphicsQueue = 0;
	m_uploadQueue = 0;
}

Engine::~Engine() {
	// TODO: use m_device->waitIdle() from ignis
	vkDeviceWaitIdle(g_device->getDevice());

	glfwTerminate();

	delete m_defaultMaterial;
	delete g_device;
}
