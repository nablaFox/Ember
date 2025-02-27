#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "device.hpp"
#include "window.hpp"
#include "renderer.hpp"
#include "mesh.hpp"
#include "default_materials.hpp"
#include "ignis/swapchain.hpp"

using namespace ignis;
using namespace ember;

Renderer::Renderer(const Window& window) {
	auto device = EmberDevice::getDevice();

	m_drawImage = ColorImage::createDrawImage({
		.device = device,
		.extent = {window.getWidth(), window.getHeight()},
		.format = EMBER_COLOR_FORMAT,
		.sampleCount = EmberDevice::getSampleCount(),
	});

	m_resolvedDrawImage = ColorImage::createDrawImage({
		.device = device,
		.extent = {window.getWidth(), window.getHeight()},
		.format = EMBER_COLOR_FORMAT,
		.sampleCount = VK_SAMPLE_COUNT_1_BIT,
	});

	m_depthImage = DepthImage::createDepthImage({
		.device = device,
		.extent = {window.getWidth(), window.getHeight()},
		.format = EMBER_DEPTH_FORMAT,
		.sampleCount = EmberDevice::getSampleCount(),
	});

	m_drawAttachment = {
		.drawImage = m_drawImage,
		.loadAction = VK_ATTACHMENT_LOAD_OP_CLEAR,
		.storeAction = VK_ATTACHMENT_STORE_OP_STORE,
		.clearColor = EMBER_CLEAR_COLOR,
	};

	m_depthAttachment = {
		.depthImage = m_depthImage,
		.loadAction = VK_ATTACHMENT_LOAD_OP_CLEAR,
		.storeAction = VK_ATTACHMENT_STORE_OP_DONT_CARE,
	};

	m_sceneDataUBO = Buffer::createUBO(device, sizeof(SceneData));

	device->registerUBO(*m_sceneDataUBO, 0);

	VkSurfaceKHR surface;

	if (glfwCreateWindowSurface(device->getInstance(), window.getWindow(), nullptr,
								&surface) != VK_SUCCESS) {
		throw std::runtime_error("failed to create window surface!");
	}

	m_swapchain = new Swapchain({
		.device = device,
		.extent = {window.getWidth(), window.getHeight()},
		.surface = surface,
		.presentMode = VK_PRESENT_MODE_MAILBOX_KHR,
	});

	for (uint32_t i = 0; i < FRAMES_IN_FLIGHT; i++) {
		frames[i].cmd = new Command(*device, EmberDevice::getGraphicsQueue());
		frames[i].finishedRendering = new Semaphore(*device);
		frames[i].waitForRenderingCompletion = new Fence(*device, true);
	}
}

void Renderer::beginScene(Camera camera, SceneInfo sceneInfo) {
	currFrame().waitForRenderingCompletion->wait();
	currFrame().waitForRenderingCompletion->reset();

	SceneData sceneData{
		.viewproj = camera.getViewProjMatrix(),
		.ambientColor = sceneInfo.ambientColor,
		.sun = sceneInfo.sun,
	};

	m_sceneDataUBO->writeData(&sceneData);

	currFrame().cmd->begin();

	currFrame().cmd->beginRender(&m_drawAttachment, &m_depthAttachment);
}

void Renderer::endScene() {
	Command* cmd = currFrame().cmd;

	cmd->endRendering();

	cmd->transitionImageLayout(*m_drawImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
	cmd->transitionImageLayout(*m_resolvedDrawImage,
							   VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

	cmd->resolveImage(*m_drawImage, *m_resolvedDrawImage);

	cmd->transitionToOptimalLayout(*m_drawImage);
	cmd->transitionToOptimalLayout(*m_resolvedDrawImage);

	cmd->end();

	SubmitCmdInfo submitCmdInfo{
		.command = cmd,
		.signalSemaphores = {currFrame().finishedRendering},
	};

	EmberDevice::getDevice()->submitCommands(
		{std::move(submitCmdInfo)}, *currFrame().waitForRenderingCompletion);

	m_swapchain->present({
		.srcImage = m_resolvedDrawImage,
		.waitSemaphores = {currFrame().finishedRendering},
	});

	m_currentFrame = (m_currentFrame + 1) % FRAMES_IN_FLIGHT;

	updateFps();
}

void Renderer::draw(Mesh& mesh, WorldTransform transform, const Material* material) {
	Command* cmd = currFrame().cmd;

	const Material* materialToUse =
		material != nullptr ? material : &defaultMaterial;

	cmd->bindPipeline(materialToUse->getPipeline());

	cmd->setViewport({
		.width = (float)m_drawImage->getExtent().width,
		.height = (float)m_drawImage->getExtent().height,
		.minDepth = 0,	// TODO handle this in ignis
		.maxDepth = 1,	// TODO handle this in ignis
	});

	cmd->setScissor(
		{0, 0, m_drawImage->getExtent().width, m_drawImage->getExtent().height});

	cmd->bindIndexBuffer(mesh.getIndexBuffer());

	m_pushConstants = {
		.worldTransform = transform.getWorldMatrix(),
		.verticesAddress = mesh.getVertexBufferAddress(),
		.materialHandle = materialToUse->getHandle(),
	};

	cmd->pushConstants(m_pushConstants);

	cmd->draw(mesh.getIndices().size());
}

void Renderer::updateFps() {
	auto currentTime = std::chrono::high_resolution_clock::now();
	float deltaTime = std::chrono::duration<float>(currentTime - m_lastTime).count();

	m_lastTime = currentTime;

	m_fps = 1 / deltaTime;
}

Renderer::~Renderer() {
	for (uint32_t i = 0; i < FRAMES_IN_FLIGHT; i++) {
		delete frames[i].cmd;
		delete frames[i].finishedRendering;
		delete frames[i].waitForRenderingCompletion;
	}

	delete m_swapchain;
	delete m_sceneDataUBO;
	delete m_drawImage;
	delete m_resolvedDrawImage;
	delete m_depthImage;
}
