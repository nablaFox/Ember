#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "window.hpp"
#include "renderer.hpp"
#include "utils.hpp"
#include "mesh.hpp"
#include "ignis/swapchain.hpp"

Renderer* Renderer::g_instance = nullptr;

Renderer::Renderer(const Window& window) {
	PRINT("renderer initialization");

	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions =
		glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char*> extensions(glfwExtensions,
										glfwExtensions + glfwExtensionCount);

	m_device = new Device({
		.extensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME},
		.instanceExtensions = extensions,
	});

	// TEMP
	graphicsQueue = 0;
	uploadQueue = 1;

	m_drawImage = ColorImage::createDrawImage(
		m_device, {window.getWidth(), window.getHeight()});

	m_depthImage = DepthImage::createDepthStencilImage(
		m_device, {window.getWidth(), window.getHeight()});

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

	m_sceneDataUBO = Buffer::createUBO<SceneData>(m_device, 1);

	VkSurfaceKHR surface;

	if (glfwCreateWindowSurface(m_device->getInstance(), window.getWindow(), nullptr,
								&surface) != VK_SUCCESS) {
		throw std::runtime_error("failed to create window surface!");
	}

	m_swapchain = new Swapchain({
		.device = m_device,
		.extent = {window.getWidth(), window.getHeight()},
		.surface = surface,
		.presentMode = VK_PRESENT_MODE_MAILBOX_KHR,
	});

	m_pipeline = new Pipeline({
		.device = m_device,
		.shaders = {"vertex.spv", "fragment.spv"},
		.colorFormat = m_drawImage->getFormat(),
		.depthFormat = m_depthImage->getFormat(),
	});

	for (uint32_t i = 0; i < FRAMES_IN_FLIGHT; i++) {
		frames[i].cmd = new Command(*m_device);
		frames[i].finishedRendering = new Semaphore(*m_device);
		frames[i].waitForRenderingCompletion = new Fence(*m_device, true);
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

	m_device->submitCommands({std::move(submitCmdInfo)},
							 *currFrame().waitForRenderingCompletion);

	m_swapchain->present({
		.image = m_drawImage,
		.waitSemaphores = {currFrame().finishedRendering},
	});

	m_currentFrame = (m_currentFrame + 1) % FRAMES_IN_FLIGHT;
}

void Renderer::draw(Mesh& mesh, WorldTransform transform) {
	Command* cmd = currFrame().cmd;

	cmd->bindIndexBuffer(mesh.getIndexBuffer());

	m_pushConstants = {
		.worldTransform = transform.getWorldMatrix().transpose(),
		.verticesAddress = mesh.getVertexBufferAddress(),
	};

	cmd->pushConstants(m_pushConstants);

	cmd->draw(mesh.getIndices().size());
}

Renderer::~Renderer() {
	PRINT("renderer cleanup");

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
	delete m_device;
}
