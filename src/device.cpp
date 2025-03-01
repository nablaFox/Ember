#include "device.hpp"
#include "types.hpp"

using namespace ignis;
using namespace ember;

EmberDevice* EmberDevice::g_instance = nullptr;

EmberDevice::EmberDevice() {
	glfwInit();

	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions =
		glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char*> extensions(glfwExtensions,
										glfwExtensions + glfwExtensionCount);

	m_device = new ignis::Device({
		.shadersFolder = EMBER_SHADERS_FOLDER,
		.extensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME},
		.instanceExtensions = extensions,
		.optionalFeatures = {"FillModeNonSolid", "SampleRateShading"},
	});

	// TEMP
	m_graphicsQueue = 0;
	m_uploadQueue = 0;
}
