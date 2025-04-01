#include "forward_renderer.hpp"
#include "fence.hpp"

using namespace ignis;
using namespace etna;

struct SceneData {
	Color ambientColor;
	DirectionalLight sun;
};

struct CameraData {
	Mat4 viewProj;
	Mat4 view;
	Mat4 proj;
};

Renderer::Renderer(const CreateInfo& info) : m_framesInFlight(info.framesInFlight) {
	assert(m_framesInFlight > 0);

	Device& device = Engine::getDevice();

	m_framesData.resize(m_framesInFlight);

	for (uint32_t i{0}; i < m_framesInFlight; i++) {
		m_framesData[i].inFlight = new ignis::Fence(device);

		m_framesData[i].cmd = new ignis::Command({
			.device = device,
			.queue = Engine::getGraphicsQueue(),
		});

		m_framesData[i].sceneDataBuff = device.createSSBO(sizeof(SceneData));
	}
}

Renderer::~Renderer() {
	Device& device = Engine::getDevice();

	for (uint32_t i{0}; i < m_framesInFlight; i++) {
		delete m_framesData[i].inFlight;
		delete m_framesData[i].cmd;
		device.destroyBuffer(m_framesData[i].sceneDataBuff);
	}
}

void Renderer::beginFrame() {
	getCommand().begin();

	m_framesData[m_currentFrame].inFlight->reset();
}

void Renderer::endFrame() {
	getCommand().end();

	SubmitCmdInfo cmdInfo{.command = getCommand()};

	Engine::getDevice().submitCommands({cmdInfo},
									   m_framesData[m_currentFrame].inFlight);

	m_framesData[m_currentFrame].inFlight->wait();

	for (const auto& cameraDataBuff : m_cameraDataBuffs) {
		Engine::getDevice().destroyBuffer(cameraDataBuff);
	}

	m_cameraDataBuffs.clear();

	m_currentFrame = (m_currentFrame + 1) % m_framesInFlight;
}

void Renderer::renderScene(const Scene& scene,
						   const RenderTarget& renderTarget,
						   const RenderSettings sceneInfo) {
#ifndef NDEBUG
	for (const auto& [_, node] : scene.getMeshes()) {
		MaterialHandle material = node.material;

		const MaterialHandle materialToUse =
			material != nullptr ? material : Engine::getDefaultMaterial();

		if (materialToUse->getTemplate().hasDepth !=
			renderTarget.getCreationInfo().hasDepth) {
			throw std::runtime_error("Material has different depth test value");
		}
	}
#endif

	Command& cmd = getCommand();

	Color clearColor = sceneInfo.clearColor;
	VkClearColorValue clearColorValue = {
		clearColor.r,
		clearColor.g,
		clearColor.b,
		clearColor.a,
	};

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

	SceneData sceneData = {
		.ambientColor = sceneInfo.ambientColor,
		.sun = sceneInfo.sun,
	};

	BufferId sceneDataBuff = m_framesData[m_currentFrame].sceneDataBuff;
	cmd.updateBuffer(sceneDataBuff, &sceneData);

	cmd.beginRender(drawAttachment, depthAttachment);

	for (const auto& [_, cameraNode] : scene.getCameras()) {
		VkViewport viewport{
			.x = cameraNode.viewport.x,
			.y = cameraNode.viewport.y,
			.width = cameraNode.viewport.width,
			.height = cameraNode.viewport.height,
			.minDepth = 0.f,
			.maxDepth = 1.f,
		};

		if (viewport.width == 0) {
			viewport.x = 0;
			viewport.width = (float)renderTarget.getExtent().width;
		}

		if (viewport.height == 0) {
			viewport.y = 0;
			viewport.height = (float)renderTarget.getExtent().height;
		}

		CameraData cameraData = {
			.viewProj =
				cameraNode.camera.getViewProjMatrix(cameraNode.getTransform()),
			.view = cameraNode.getTransform().getViewMatrix(),
			.proj = cameraNode.camera.getProjMatrix(),
		};

		BufferId cameraDataBuff =
			Engine::getDevice().createUBO(sizeof(CameraData), &cameraData);

		m_cameraDataBuffs.push_back(cameraDataBuff);

		for (const auto& [_, meshNode] : scene.getMeshes()) {
			MeshHandle mesh = meshNode.mesh;
			MaterialHandle material = meshNode.material;

			const MaterialHandle materialToUse =
				material != nullptr ? material : Engine::getDefaultMaterial();

			Pipeline& pipeline = materialToUse->getTemplate().getPipeline();

			cmd.bindPipeline(pipeline);

			cmd.setViewport(viewport);

			cmd.setScissor({0, 0, renderTarget.getExtent().width,
							renderTarget.getExtent().height});

			cmd.bindIndexBuffer(*mesh->getIndexBuffer());

			m_pushConstants = {
				.worldTransform = meshNode.getWorldMatrix(),
				.vertices = mesh->getVertexBuffer(),
				.material = materialToUse->getParamsUBO(),
				.sceneData = sceneDataBuff,
				.cameraData = cameraDataBuff,
			};

			cmd.pushConstants(pipeline, m_pushConstants);

			cmd.draw(mesh->indexCount());
		}
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
