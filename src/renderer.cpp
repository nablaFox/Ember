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

	m_cmd = new Command(*m_device);

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

	m_finishedRendering = new Semaphore(*m_device);
	m_waitForRenderingCompletion = new Fence(*m_device, true);
}

void Renderer::beginScene(Camera camera, DirectionalLight sun, Color ambientColor) {
	m_waitForRenderingCompletion->wait();
	m_waitForRenderingCompletion->reset();

	SceneData sceneData{
		.viewproj = camera.getViewProjMatrix().transpose(),
		.ambientColor = ambientColor,
		.sun = sun,
	};

	m_sceneDataUBO->writeData(&sceneData);

	m_cmd->begin();

	m_cmd->bindPipeline(*m_pipeline);

	m_cmd->bindUBO(*m_sceneDataUBO, 0, 0);

	m_cmd->setViewport({0, 0, (float)m_drawImage->getExtent().width,
						(float)m_drawImage->getExtent().height});

	m_cmd->setScissor(
		{0, 0, m_drawImage->getExtent().width, m_drawImage->getExtent().height});

	m_cmd->beginRender(&m_drawAttachment, &m_depthAttachment);
}

void Renderer::endScene() {
	m_cmd->endRendering();

	m_cmd->end();

	SubmitCmdInfo submitCmdInfo{
		.command = m_cmd,
		.signalSemaphores = {m_finishedRendering},
	};

	m_device->submitCommands({std::move(submitCmdInfo)},
							 *m_waitForRenderingCompletion);

	m_swapchain->present({
		.image = m_drawImage,
		.waitSemaphores = {m_finishedRendering},
	});
}

void Renderer::draw(Mesh& mesh, WorldTransform transform) {
	m_cmd->bindIndexBuffer(mesh.getIndexBuffer());

	m_pushConstants = {
		.worldTransform = transform.getWorldMatrix().transpose(),
		.verticesAddress = mesh.getVertexBufferAddress(),
	};

	m_cmd->pushConstants(m_pushConstants);

	m_cmd->draw(mesh.getIndices().size());
}

Renderer::~Renderer() {
	PRINT("renderer cleanup");

	delete m_swapchain;
	delete m_finishedRendering;
	delete m_waitForRenderingCompletion;
	delete m_sceneDataUBO;
	delete m_pipeline;
	delete m_cmd;
	delete m_drawImage;
	delete m_depthImage;
	delete m_device;
}
