#include "window.hpp"
#include "renderer.hpp"
#include "utils.hpp"
#include "mesh.hpp"

Renderer::Renderer(const Window& window) {
	PRINT("renderer initialization");

	m_device = new Device({
		.extensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME},
	});

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

	// create swapchain

	m_pipeline = new Pipeline({
		.device = m_device,
		.shaders = {"vertex.spv", "fragment.spv"},
		.colorFormat = m_drawImage->getFormat(),
		.depthFormat = m_depthImage->getFormat(),
	});
}

void Renderer::beginScene(SceneData data) {
	return;
	m_sceneDataUBO->writeData(&data);

	m_cmd->begin();

	m_cmd->bindPipeline(*m_pipeline);

	m_cmd->bindBuffer(*m_sceneDataUBO, 0, 0);

	m_cmd->beginRender(m_drawAttachment, m_depthAttachment);
}

void Renderer::endScene() {
	return;
	m_cmd->end();

	// submit commands

	// copy draw image in swapchain

	// present swapchain image
}

auto Renderer::getFrameTime() -> float {
	return 0.f;
}

void Renderer::draw(const Mesh& mesh, WorldTransform transform) {
	return;
	m_cmd->bindIndexBuffer(*mesh.getIndexBuffer());

	PushConstants pushConstants{
		.verticesAddress = mesh.getVertexBufferAddress(),
		// .worldTransform = getWorldMatrix(transform)
	};

	m_cmd->pushConstants(pushConstants);
}

Renderer::~Renderer() {
	PRINT("renderer cleanup");

	delete m_sceneDataUBO;
	delete m_pipeline;
	delete m_cmd;
	delete m_drawImage;
	delete m_depthImage;
	delete m_device;
}
