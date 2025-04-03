#include "renderer.hpp"
#include "engine.hpp"
#include "default_materials.hpp"

using namespace etna;
using namespace ignis;

MaterialHandle g_defaultMaterial{nullptr};

Renderer::Renderer(const CreateInfo& info) : m_framesInFlight(info.framesInFlight) {
	assert(m_framesInFlight > 0);

	m_frames.resize(m_framesInFlight);

	for (uint32_t i{0}; i < m_framesInFlight; i++) {
		m_frames[i].inFlight = new ignis::Fence(_device);

		m_frames[i].cmd = new ignis::Command({
			.device = _device,
			.queue = engine::getGraphicsQueue(),
		});
	}

	g_defaultMaterial = engine::createColorMaterial(WHITE);
}

Renderer::~Renderer() {
	for (uint32_t i{0}; i < m_framesInFlight; i++) {
		delete m_frames[i].inFlight;
		delete m_frames[i].cmd;
	}

	g_defaultMaterial.reset();
}

void Renderer::beginFrame(const RenderTarget& target,
						  const RenderFrameSettings& settings) {
	Command& cmd = getCommand();
	FrameData& frame = m_frames[m_currentFrame];

	m_currTarget = &target;

	frame.inFlight->reset();

	cmd.begin();

	const VkClearColorValue clearColorValue{
		settings.clearColor.r,
		settings.clearColor.g,
		settings.clearColor.b,
		settings.clearColor.a,
	};

	DrawAttachment* drawAttachment = new DrawAttachment({
		.drawImage = m_currTarget->getDrawImage(),
		.loadAction = settings.colorLoadOp,
		.storeAction = settings.colorStoreOp,
		.clearColor = clearColorValue,
	});

	DepthAttachment* depthAttachment =
		settings.renderDepth ? new DepthAttachment({
								   .depthImage = m_currTarget->getDepthImage(),
								   .loadAction = settings.depthLoadOp,
								   .storeAction = settings.depthStoreOp,
							   })
							 : nullptr;

	cmd.beginRender(drawAttachment, depthAttachment);
}

void Renderer::endFrame() {
	Command& cmd = getCommand();

	cmd.endRendering();

	if (!m_currTarget->isMultiSampled())
		return;

	ignis::Image& drawImage = *m_currTarget->getDrawImage();
	ignis::Image& resolvedDrawImage = *m_currTarget->getResolvedImage();

	cmd.transitionImageLayout(drawImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
	cmd.transitionImageLayout(resolvedDrawImage,
							  VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

	cmd.resolveImage(drawImage, resolvedDrawImage);

	cmd.transitionToOptimalLayout(drawImage);
	cmd.transitionToOptimalLayout(resolvedDrawImage);

	cmd.end();

	SubmitCmdInfo cmdInfo{.command = cmd};

	_device.submitCommands({cmdInfo}, m_frames[m_currentFrame].inFlight);

	m_frames[m_currentFrame].inFlight->wait();

	m_currentFrame = (m_currentFrame + 1) % m_framesInFlight;

	for (auto& buff : m_deletionQueue) {
		_device.destroyBuffer(buff);
	}

	m_deletionQueue.clear();
}

void Renderer::drawInstanced(uint32_t instanceCount,
							 const MeshHandle mesh,
							 const MaterialHandle material,
							 const Mat4& transform,
							 const RenderSettings& settings) {
	Command& cmd = getCommand();

	assert(mesh != nullptr && "Mesh is null");

	VkViewport vp{
		.x = settings.viewport.x,
		.y = settings.viewport.y,
		.width = settings.viewport.width,
		.height = settings.viewport.height,
		.minDepth = 0.f,
		.maxDepth = 1.f,
	};

	const MaterialHandle materialToUse =
		material != nullptr ? material : g_defaultMaterial;

	Pipeline& pipeline = materialToUse->getTemplate().getPipeline();

	cmd.bindPipeline(pipeline);

	cmd.setViewport(vp);

	cmd.setScissor(
		{0, 0, m_currTarget->getExtent().width, m_currTarget->getExtent().height});

	cmd.bindIndexBuffer(*mesh->getIndexBuffer());

	m_pushConstants = {
		.worldTransform = transform,
		.vertices = mesh->getVertexBuffer(),
		.material = materialToUse->getParamsUBO(),
		.instanceBuffer = settings.instanceBuffer,
		.ubo = settings.ubo,
		.ssbo = settings.ssbo,
	};

	cmd.pushConstants(pipeline, m_pushConstants);

	if (instanceCount == 1)
		cmd.draw(mesh->indexCount());
	else
		cmd.drawInstanced(instanceCount, mesh->indexCount());
}

void Renderer::draw(const MeshHandle mesh,
					const MaterialHandle material,
					const Mat4& transform,
					const RenderSettings& settings) {
	drawInstanced(1, mesh, material, transform, settings);
}

void Renderer::clearViewport(Viewport vp, Color color) {
	// TODO: use ignis utility

	const VkClearColorValue clearColorValue{
		color.r,
		color.g,
		color.b,
		color.a,
	};

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
		.clearValue = {clearColorValue},
	};

	vkCmdClearAttachments(getCommand().getHandle(), 1, &clearAtt, 1, &clearRect);
}

void Renderer::queueForDeletion(ignis::BufferId buffer) {
	m_deletionQueue.push_back(buffer);
}
