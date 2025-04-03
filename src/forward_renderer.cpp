#include "forward_renderer.hpp"
#include "ignis/fence.hpp"
#include "default_materials.hpp"
#include "mesh.hpp"
#include "engine.hpp"

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

void Renderer::begin() {
	getCommand().begin();

	m_framesData[m_currentFrame].inFlight->reset();
}

void Renderer::execute() {
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

void Renderer::beginDrawing(const RenderTarget& renderTarget,
							const CameraNode& cameraNode,
							const RenderFrameSettings& info) {
	Command& cmd = getCommand();
	FrameData& frameData = m_framesData[m_currentFrame];

	const SceneData sceneData{
		.ambientColor = info.ambientColor,
		.sun = info.sun,
	};

	cmd.updateBuffer(frameData.sceneDataBuff, &sceneData);

	m_currCamera = &cameraNode;
	m_currTarget = &renderTarget;

	const VkClearColorValue clearColorValue{
		info.clearColor.r,
		info.clearColor.g,
		info.clearColor.b,
		info.clearColor.a,
	};

	DrawAttachment* drawAttachment = new DrawAttachment({
		.drawImage = renderTarget.getDrawImage(),
		.loadAction = info.colorLoadOp,
		.storeAction = info.colorStoreOp,
		.clearColor = clearColorValue,
	});

	DepthAttachment* depthAttachment =
		info.renderDepth ? new DepthAttachment({
							   .depthImage = renderTarget.getDepthImage(),
							   .loadAction = info.depthLoadOp,
							   .storeAction = info.depthStoreOp,
						   })
						 : nullptr;

	cmd.beginRender(drawAttachment, depthAttachment);
}

void Renderer::endDrawing() {
	Command& cmd = getCommand();

	cmd.endRendering();

	if (!m_currTarget->isMultiSampled())
		return;

	Image& drawImage = *m_currTarget->getDrawImage();
	Image& resolvedDrawImage = *m_currTarget->getResolvedImage();

	cmd.transitionImageLayout(drawImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
	cmd.transitionImageLayout(resolvedDrawImage,
							  VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

	cmd.resolveImage(drawImage, resolvedDrawImage);

	cmd.transitionToOptimalLayout(drawImage);
	cmd.transitionToOptimalLayout(resolvedDrawImage);
}

// TODO: group meshes by material
void Renderer::renderScene(const Scene& scene, const RenderSettings& info) {
#ifndef NDEBUG
	for (const auto& [_, node] : scene.getMeshes()) {
		const MaterialTemplate& materialT = node.material != nullptr
												? node.material->getTemplate()
												: g_defaultMaterial->getTemplate();

		const RenderTarget::CreateInfo& renderTargetInfo =
			m_currTarget->getCreationInfo();

		if (materialT.samples != renderTargetInfo.samples) {
			throw std::runtime_error("Material has different sample count");
		}
	}
#endif

	Command& cmd = getCommand();

	VkViewport vp{
		.x = info.viewport.x,
		.y = info.viewport.y,
		.width = info.viewport.width,
		.height = info.viewport.height,
		.minDepth = 0.f,
		.maxDepth = 1.f,
	};

	if (vp.width == 0) {
		vp.x = 0;
		vp.width = (float)m_currTarget->getExtent().width;
	}

	if (vp.height == 0) {
		vp.y = 0;
		vp.height = (float)m_currTarget->getExtent().height;
	}

	const Mat4 view = m_currCamera->getViewMatrix();

	const Mat4 proj = m_currCamera->getProjMatrix(vp.width / vp.height);

	const CameraData cameraData{
		.viewProj = proj * view,
		.view = view,
		.proj = proj,
	};

	BufferId cameraDataBuff = _device.createUBO(sizeof(CameraData), &cameraData);

	m_cameraDataBuffs.push_back(cameraDataBuff);

	// TODO: add ignis utility for clearing the viewport
	if (info.clearViewport) {
		const VkClearRect clearRect{
			.rect =
				{
					{(int32_t)vp.x, (int32_t)vp.y},
					{(uint32_t)vp.width, (uint32_t)vp.height},
				},
			.layerCount = 1,
		};

		const VkClearAttachment clearAtt{
			.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
			.clearValue =
				{
					.color = {info.clearColor.r, info.clearColor.g,
							  info.clearColor.b, info.clearColor.a},
				},
		};

		vkCmdClearAttachments(getCommand().getHandle(), 1, &clearAtt, 1, &clearRect);
	}

	for (const auto& [_, meshNode] : scene.getMeshes()) {
		MeshHandle mesh = meshNode.mesh;
		MaterialHandle material = meshNode.material;

		if (mesh == nullptr)
			continue;

		const MaterialHandle materialToUse =
			material != nullptr ? material : g_defaultMaterial;

		assert(materialToUse != nullptr);

		Pipeline& pipeline = materialToUse->getTemplate().getPipeline();

		cmd.bindPipeline(pipeline);

		cmd.setViewport(vp);

		cmd.setScissor({0, 0, m_currTarget->getExtent().width,
						m_currTarget->getExtent().height});

		cmd.bindIndexBuffer(*mesh->getIndexBuffer());

		m_pushConstants = {
			.worldTransform = meshNode.getWorldMatrix(),
			.vertices = mesh->getVertexBuffer(),
			.material = materialToUse->getParamsUBO(),
			.sceneData = m_framesData[m_currentFrame].sceneDataBuff,
			.cameraData = cameraDataBuff,
		};

		cmd.pushConstants(pipeline, m_pushConstants);

		cmd.draw(mesh->indexCount());
	}
}

void Renderer::drawScene(const RenderTarget& target,
						 const CameraNode& cameraNode,
						 const Scene& scene,
						 const Viewport& viewport,
						 const RenderFrameSettings& settings) {
	beginDrawing(target, cameraNode, settings);

	renderScene(scene, {.viewport = viewport, .clearViewport = false});

	endDrawing();
}
