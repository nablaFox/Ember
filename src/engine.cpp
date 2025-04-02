#include <GLFW/glfw3.h>
#include <deque>
#include "ignis/device.hpp"
#include "ignis/command.hpp"
#include "ignis/fence.hpp"
#include "engine.hpp"

using namespace etna;
using namespace ignis;

Device* g_device{nullptr};
VkQueue g_graphicsQueue{nullptr};
VkQueue g_uploadQueue{nullptr};

std::deque<std::function<void()>> g_deletionQueue;

static void shutdown() {
	g_device->waitIdle();

	glfwTerminate();

	for (auto& func : g_deletionQueue) {
		func();
	}

	delete g_device;
}

void engine::init() {
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

	// TODO: choose graphics & upload queues
	g_graphicsQueue = g_device->getQueue(0);
	g_uploadQueue = g_device->getQueue(0);

	std::atexit(shutdown);
}

Device& engine::getDevice() {
	if (g_device != nullptr)
		return *g_device;

	throw std::runtime_error("Engine must be initialized to access the GPU device");
}

void engine::queueForDeletion(std::function<void()> func) {
	g_deletionQueue.push_back(func);
}

VkQueue engine::getGraphicsQueue() {
	if (g_device != nullptr)
		return g_graphicsQueue;

	throw std::runtime_error(
		"Engine must be initialized to access the graphics queue");
}

VkQueue engine::getUploadQueue() {
	if (g_device != nullptr)
		return g_uploadQueue;

	throw std::runtime_error(
		"Engine must be initialized to access the upload queue");
}

uint32_t engine::getMaxAllowedSampleCount() {
	if (g_device != nullptr) {
		uint32_t deviceMaxSampleCount = g_device->getMaxSampleCount();

		return deviceMaxSampleCount > ETNA_MAX_SAMPLE_COUNT ? ETNA_MAX_SAMPLE_COUNT
															: deviceMaxSampleCount;
	}

	throw std::runtime_error(
		"Engine must be initialized to access the maximum sample count");
}
