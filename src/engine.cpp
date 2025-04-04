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
VkQueue g_immediateQueue{nullptr};

std::deque<std::function<void()>> g_deletionQueue;

static void checkInit() {
	if (g_device == nullptr) {
		throw std::runtime_error(
			"Engine must be initialized to access the GPU device");
	}
}

static void cleanup() {
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
	g_immediateQueue = g_device->getQueue(0);

	std::atexit(cleanup);
}

Device& engine::getDevice() {
	checkInit();

	return *g_device;
}

void engine::immediateSubmit(std::function<void(ignis::Command&)>&& func) {
	checkInit();

	Command cmd{{
		.device = *g_device,
		.queue = g_graphicsQueue,
	}};

	cmd.begin();

	func(cmd);

	cmd.end();

	g_device->submitCommands({{.command = cmd}}, nullptr);
	g_device->waitIdle();
}

void engine::immediateUpdate(ignis::BufferId buffer,
							 const void* data,
							 VkDeviceSize offset,
							 VkDeviceSize size) {
	immediateSubmit(
		[&](ignis::Command& cmd) { cmd.updateBuffer(buffer, data, offset, size); });
}

ignis::Command engine::createGraphicsCommand() {
	checkInit();

	return Command({.device = *g_device, .queue = g_graphicsQueue});
}

ignis::Command* engine::newGraphicsCommand() {
	checkInit();

	return new Command({.device = *g_device, .queue = g_graphicsQueue});
}

void engine::queueForDeletion(std::function<void()> func) {
	checkInit();

	g_deletionQueue.push_back(func);
}

uint32_t engine::clampSampleCount(uint32_t sampleCount) {
	checkInit();

	if (sampleCount == 0) {
		return MAX_SAMPLE_COUNT;
	}

	const uint32_t deviceMaxSampleCount = g_device->getMaxSampleCount();

	return deviceMaxSampleCount > MAX_SAMPLE_COUNT ? MAX_SAMPLE_COUNT
												   : deviceMaxSampleCount;
}
