#include <GLFW/glfw3.h>
#include "ignis/device.hpp"
#include "ignis/command.hpp"
#include "ignis/fence.hpp"
#include "engine.hpp"

using namespace etna;
using namespace ignis;

Device* g_device{nullptr};
VkQueue g_graphicsQueue{nullptr};
VkQueue g_uploadQueue{nullptr};

// Materials
MaterialHandle g_defaultMaterial{nullptr};
MaterialHandle g_pointMaterial{nullptr};

MaterialTemplateHandle g_gridTemplate{nullptr};
MaterialTemplateHandle g_transparentGridTemplate{nullptr};
MaterialTemplateHandle g_brickOutlineMaterialTemplate{nullptr};

Engine::Engine() {
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
}

Engine::~Engine() {
	g_device->waitIdle();

	glfwTerminate();

	g_defaultMaterial.reset();
	g_pointMaterial.reset();
	g_gridTemplate.reset();
	g_transparentGridTemplate.reset();
	g_brickOutlineMaterialTemplate.reset();

	delete g_device;
}

Device& Engine::getDevice() {
	if (g_device != nullptr)
		return *g_device;

	throw std::runtime_error("Engine must be initialized to access the GPU device");
}

VkQueue Engine::getGraphicsQueue() {
	if (g_device != nullptr)
		return g_graphicsQueue;

	throw std::runtime_error(
		"Engine must be initialized to access the graphics queue");
}

VkQueue Engine::getUploadQueue() {
	if (g_device != nullptr)
		return g_uploadQueue;

	throw std::runtime_error(
		"Engine must be initialized to access the upload queue");
}

uint32_t Engine::getMaxAllowedSampleCount() {
	if (g_device != nullptr) {
		uint32_t deviceMaxSampleCount = g_device->getMaxSampleCount();

		return deviceMaxSampleCount > ETNA_MAX_SAMPLE_COUNT ? ETNA_MAX_SAMPLE_COUNT
															: deviceMaxSampleCount;
	}

	throw std::runtime_error(
		"Engine must be initialized to access the maximum sample count");
}

// PONDER: maybe use a macro or an helper function to reduce duplication

MaterialHandle Engine::getDefaultMaterial() {
	if (g_defaultMaterial == nullptr) {
		g_defaultMaterial = Material::create({
			.shaders = {"default.vert.spv", "default.frag.spv"},
		});
	}

	return g_defaultMaterial;
}

MaterialHandle Engine::getPointMaterial() {
	if (g_pointMaterial == nullptr) {
		g_pointMaterial = Material::create({
			.shaders = {"default.vert.spv", "default.frag.spv"},
			.polygonMode = VK_POLYGON_MODE_POINT,
		});
	}

	return g_pointMaterial;
}

MaterialHandle Engine::createGridMaterial(GridMaterialParams params) {
	if (g_gridTemplate == nullptr) {
		g_gridTemplate = MaterialTemplate::create({
			.shaders = {"default.vert.spv", "grid.frag.spv"},
			.paramsSize = sizeof(GridMaterialParams),
		});
	}

	return Material::create({
		.templateHandle = g_gridTemplate,
		.params = &params,
	});
}

MaterialHandle Engine::createTransparentGridMaterial(GridMaterialParams params) {
	if (g_transparentGridTemplate == nullptr) {
		g_transparentGridTemplate = MaterialTemplate::create({
			.shaders = {"default.vert.spv", "grid.frag.spv"},
			.paramsSize = sizeof(GridMaterialParams),
			.transparency = true,
		});
	}

	return Material::create({
		.templateHandle = g_transparentGridTemplate,
		.params = &params,
	});
}

MaterialHandle Engine::createBrickOutlinedMaterial(OutlineMaterialParams params) {
	if (g_brickOutlineMaterialTemplate == nullptr) {
		g_brickOutlineMaterialTemplate = MaterialTemplate::create({
			.shaders = {"default.vert.spv", "brick_outline.frag.spv"},
			.paramsSize = sizeof(OutlineMaterialParams),
		});
	}

	return Material::create({
		.templateHandle = g_brickOutlineMaterialTemplate,
		.params = &params,
	});
}
