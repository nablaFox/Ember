#include <GLFW/glfw3.h>
#include "ignis/device.hpp"
#include "ignis/command.hpp"
#include "ignis/fence.hpp"
#include "engine.hpp"
#include "scene.hpp"
#include "camera.hpp"
#include "render_target.hpp"

using namespace etna;
using namespace ignis;

Device* g_device = nullptr;
VkQueue g_graphicsQueue;
VkQueue g_uploadQueue;

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

	m_defaultMaterial =
		new _Material({.shaders = {"default.vert.spv", "default.frag.spv"}});

	// TODO: choose graphics & upload queues
	g_graphicsQueue = g_device->getQueue(0);
	g_uploadQueue = g_device->getQueue(0);

	for (uint32_t i{0}; i < ETNA_FRAMES_IN_FLIGHT; i++) {
		m_framesData[i].m_inFlight = new ignis::Fence(*g_device);

		m_framesData[i].m_cmd = new ignis::Command({
			.device = *g_device,
			.queue = g_graphicsQueue,
		});
	}
}

Engine::~Engine() {
	g_device->waitIdle();

	glfwTerminate();

	for (uint32_t i{0}; i < ETNA_FRAMES_IN_FLIGHT; i++) {
		delete m_framesData[i].m_inFlight;
		delete m_framesData[i].m_cmd;
	}

	delete m_defaultMaterial;
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

void Engine::beginFrame() {
	getCommand().begin();

	m_framesData[m_currentFrame].m_inFlight->reset();
}

void Engine::endFrame() {
	getCommand().end();

	SubmitCmdInfo cmdInfo{.command = getCommand()};

	g_device->submitCommands({cmdInfo}, m_framesData[m_currentFrame].m_inFlight);

	m_framesData[m_currentFrame].m_inFlight->wait();

	m_currentFrame = (m_currentFrame + 1) % ETNA_FRAMES_IN_FLIGHT;
}

void Engine::renderScene(const Scene& scene,
						 const RenderTarget& renderTarget,
						 const Camera& camera,
						 const RenderSettings sceneInfo) {
	Command& cmd = getCommand();
	VkViewport viewport = sceneInfo.viewport;
	Color clearColor = sceneInfo.clearColor;

	// update current scene data ubo
	BufferId sceneDataBuff = scene.getSceneDataBuff(m_currentFrame);

	SceneData sceneData = {
		.viewproj = camera.getViewProjMatrix(),
	};

	cmd.updateBuffer(sceneDataBuff, &sceneData);

	VkClearColorValue clearColorValue = {clearColor.r, clearColor.g, clearColor.b,
										 clearColor.a};

	DrawAttachment* drawAttachment = new DrawAttachment({
		.drawImage = renderTarget.getDrawImage(),
		.loadAction = sceneInfo.colorLoadOp,
		.storeAction = sceneInfo.colorStoreOp,
		.clearColor = clearColorValue,
	});

	DepthAttachment* depthAttachment =
		renderTarget.getCreationInfo().hasDepth
			? new DepthAttachment({
				  .depthImage = renderTarget.getDepthImage(),
				  .loadAction = sceneInfo.depthLoadOp,
				  .storeAction = sceneInfo.depthStoreOp,
			  })
			: nullptr;

	cmd.beginRender(drawAttachment, depthAttachment);

#ifndef NDEBUG
	for (const auto& node : scene.getNodes()) {
		Material material = node.material;

		const _Material* materialToUse =
			material != nullptr ? material.get() : m_defaultMaterial;

		if (materialToUse->hasDepthTest() !=
			renderTarget.getCreationInfo().hasDepth) {
			throw std::runtime_error("Material has different depth test value");
		}
	}
#endif

	for (const auto& node : scene.getNodes()) {
		Mesh mesh = node.mesh;
		Material material = node.material;

		const _Material* materialToUse =
			material != nullptr ? material.get() : m_defaultMaterial;

		Pipeline& pipeline = materialToUse->getPipeline();

		cmd.bindPipeline(pipeline);

		if (viewport.width == 0) {
			viewport.x = 0;
			viewport.width = (float)renderTarget.getExtent().width;
		}

		if (viewport.height == 0) {
			viewport.y = 0;
			viewport.height = (float)renderTarget.getExtent().height;
		}

		viewport.maxDepth = 1.f;
		viewport.minDepth = 0.f;

		cmd.setViewport(viewport);

		cmd.setScissor(
			{0, 0, renderTarget.getExtent().width, renderTarget.getExtent().height});

		cmd.bindIndexBuffer(*mesh->getIndexBuffer());

		m_pushConstants = {
			.worldTransform = node.transform.getWorldMatrix(),
			.vertices = mesh->getVertexBuffer(),
			.material = materialToUse->getParamsUBO(),
			.sceneData = sceneDataBuff,
		};

		cmd.pushConstants(pipeline, m_pushConstants);

		cmd.draw(mesh->indexCount());
	}

	cmd.endRendering();

	if (!renderTarget.isMultiSampled())
		return;

	Image& drawImage = *renderTarget.getDrawImage();
	Image& resolvedDrawImage = *renderTarget.getResolvedImage();

	cmd.transitionImageLayout(drawImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
	cmd.transitionImageLayout(resolvedDrawImage,
							  VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

	cmd.resolveImage(drawImage, resolvedDrawImage);

	cmd.transitionToOptimalLayout(drawImage);
	cmd.transitionToOptimalLayout(resolvedDrawImage);
}

Mesh Engine::createMesh(const _Mesh::CreateInfo& info) {
	return std::shared_ptr<_Mesh>(new _Mesh(info));
}

Material Engine::createMaterial(const _Material::CreateInfo& info) {
	return std::shared_ptr<_Material>(new _Material(info));
}
