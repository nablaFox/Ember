#include "forward_renderer.hpp"
#include "ignis/fence.hpp"
#include "default_materials.hpp"
#include "mesh.hpp"

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

MaterialHandle g_defaultMaterial{nullptr};

Renderer::Renderer(const CreateInfo& info) : m_framesInFlight(info.framesInFlight) {
	assert(m_framesInFlight > 0);

	m_framesData.resize(m_framesInFlight);

	for (uint32_t i{0}; i < m_framesInFlight; i++) {
		m_framesData[i].inFlight = new ignis::Fence(_device);

		m_framesData[i].cmd = new ignis::Command({
			.device = _device,
			.queue = engine::getGraphicsQueue(),
		});

		m_framesData[i].sceneDataBuff = _device.createSSBO(sizeof(SceneData));
	}

	g_defaultMaterial = engine::createColorMaterial(WHITE);
}

Renderer::~Renderer() {
	for (uint32_t i{0}; i < m_framesInFlight; i++) {
		delete m_framesData[i].inFlight;
		delete m_framesData[i].cmd;
		_device.destroyBuffer(m_framesData[i].sceneDataBuff);
	}

	g_defaultMaterial.reset();
}

void Renderer::beginFrame() {
	getCommand().begin();

	m_framesData[m_currentFrame].inFlight->reset();
}

void Renderer::endFrame() {
	getCommand().end();

	SubmitCmdInfo cmdInfo{.command = getCommand()};

	_device.submitCommands({cmdInfo}, m_framesData[m_currentFrame].inFlight);

	m_framesData[m_currentFrame].inFlight->wait();

	for (const auto& cameraDataBuff : m_cameraDataBuffs) {
		_device.destroyBuffer(cameraDataBuff);
	}

	m_cameraDataBuffs.clear();

	m_currentFrame = (m_currentFrame + 1) % m_framesInFlight;
}

// TODO: group meshes by material
// PONDER: for multi-pass rendering use a render graph where we will automatically
// handle syncrhonization between resources
void Renderer::renderScene(const Scene& scene,
						   const RenderTarget& renderTarget,
						   const RenderSettings sceneInfo) {
#ifndef NDEBUG
	for (const auto& [_, node] : scene.getMeshes()) {
		const MaterialTemplate& materialT = node.material != nullptr
												? node.material->getTemplate()
												: g_defaultMaterial->getTemplate();

		const RenderTarget::CreateInfo& renderTargetInfo =
			renderTarget.getCreationInfo();

		if (materialT.hasDepth != renderTargetInfo.hasDepth) {
			throw std::runtime_error("Material has different depth test value");
		}

		if (materialT.samples != renderTargetInfo.samples) {
			throw std::runtime_error("Material has different sample count");
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
				cameraNode.camera.getViewProjMatrix(cameraNode.getWorldMatrix()),
			.view = cameraNode.getTransform().getViewMatrix(),
			.proj = cameraNode.camera.getProjMatrix(),
		};

		BufferId cameraDataBuff = _device.createUBO(sizeof(CameraData), &cameraData);

		m_cameraDataBuffs.push_back(cameraDataBuff);

		for (const auto& [_, meshNode] : scene.getMeshes()) {
			MeshHandle mesh = meshNode.mesh;
			MaterialHandle material = meshNode.material;

			if (mesh == nullptr)
				continue;

			const MaterialHandle materialToUse =
				material != nullptr ? material : g_defaultMaterial;

			assert(mesh != nullptr);
			assert(materialToUse != nullptr);

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

	// PONDER: the resolving should probably not happen here
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
