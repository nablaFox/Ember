#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "device.hpp"
#include "window.hpp"
#include "renderer.hpp"
#include "mesh.hpp"
#include "ignis/swapchain.hpp"

Renderer::Renderer(const Window& window) {
	auto device = EmberDevice::getDevice();

	// TODO: ignis should allow for specifying the color format
	m_drawImage =
		ColorImage::createDrawImage(device, {window.getWidth(), window.getHeight()});

	// TODO: ignis should allow for the depth format
	m_depthImage = DepthImage::createDepthStencilImage(
		device, {window.getWidth(), window.getHeight()});

	m_drawAttachment = {
		.drawImage = m_drawImage,
		.loadAction = VK_ATTACHMENT_LOAD_OP_CLEAR,
		.storeAction = VK_ATTACHMENT_STORE_OP_STORE,
	};

	m_depthAttachment = {
		.depthImage = m_depthImage,
		.loadAction = VK_ATTACHMENT_LOAD_OP_CLEAR,
		.storeAction = VK_ATTACHMENT_STORE_OP_DONT_CARE,
	};

	m_sceneDataUBO = Buffer::createUBO<SceneData>(device, 1);

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

	m_pipeline = new Pipeline({
		.device = device,
		.shaders = {"default.vert.spv", "default.frag.spv"},
		.colorFormat = m_drawImage->getFormat(),
		.depthFormat = m_depthImage->getFormat(),
	});

	for (uint32_t i = 0; i < FRAMES_IN_FLIGHT; i++) {
		frames[i].cmd = new Command(*device, EmberDevice::getGraphicsQueue());
		frames[i].finishedRendering = new Semaphore(*device);
		frames[i].waitForRenderingCompletion = new Fence(*device, true);
	}
}

void Renderer::beginScene(Camera camera, DirectionalLight sun, Color ambientColor) {
	currFrame().waitForRenderingCompletion->wait();
	currFrame().waitForRenderingCompletion->reset();

	SceneData sceneData{
		.viewproj = camera.getViewProjMatrix().transpose(),
		.ambientColor = ambientColor,
		.sun = sun,
	};

	m_sceneDataUBO->writeData(&sceneData);

	Command* cmd = currFrame().cmd;

	cmd->begin();

	cmd->bindPipeline(*m_pipeline);

	cmd->bindUBO(*m_sceneDataUBO, 0, 0);

	cmd->setViewport({0, 0, (float)m_drawImage->getExtent().width,
					  (float)m_drawImage->getExtent().height});

	cmd->setScissor(
		{0, 0, m_drawImage->getExtent().width, m_drawImage->getExtent().height});

	cmd->beginRender(&m_drawAttachment, &m_depthAttachment);
}

void Renderer::endScene() {
	Command* cmd = currFrame().cmd;

	cmd->endRendering();

	cmd->end();

	SubmitCmdInfo submitCmdInfo{
		.command = cmd,
		.signalSemaphores = {currFrame().finishedRendering},
	};

	EmberDevice::getDevice()->submitCommands(
		{std::move(submitCmdInfo)}, *currFrame().waitForRenderingCompletion);

	m_swapchain->present({
		.image = m_drawImage,
		.waitSemaphores = {currFrame().finishedRendering},
	});

	m_currentFrame = (m_currentFrame + 1) % FRAMES_IN_FLIGHT;

	updateFps();
}

void Renderer::draw(Mesh& mesh, WorldTransform transform) {
	Command* cmd = currFrame().cmd;

	cmd->bindIndexBuffer(mesh.getIndexBuffer());

	m_pushConstants = {
		.worldTransform = transform.getWorldMatrix()
							  .transpose(),	 // CHECK we should probably adopt column
											 // major order for performance
		.verticesAddress = mesh.getVertexBufferAddress(),
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
	delete m_pipeline;
	delete m_drawImage;
	delete m_depthImage;
}
