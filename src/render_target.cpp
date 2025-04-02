#include "engine.hpp"
#include "render_target.hpp"

using namespace ignis;
using namespace etna;

RenderTarget::RenderTarget(const CreateInfo& info) : m_creationInfo(info) {
	const uint32_t maxSampleCount = engine::getMaxAllowedSampleCount();

	const uint32_t sampleCount = (info.samples == 0 || info.samples > maxSampleCount)
									 ? maxSampleCount
									 : info.samples;

	m_creationInfo.samples = sampleCount;

	const VkSampleCountFlagBits sampleCountBits =
		static_cast<VkSampleCountFlagBits>(sampleCount);

	m_drawImage = new Image(_device.createDrawAttachmentImage({
		.width = info.extent.width,
		.height = info.extent.height,
		.format = engine::ETNA_COLOR_FORMAT,
		.sampleCount = sampleCountBits,
	}));

	if (isMultiSampled()) {
		m_resolvedImage = new Image(_device.createDrawAttachmentImage({
			.width = info.extent.width,
			.height = info.extent.height,
			.format = engine::ETNA_COLOR_FORMAT,
			.sampleCount = VK_SAMPLE_COUNT_1_BIT,
		}));
	}

	m_depthImage = new Image(_device.createDepthAttachmentImage({
		.width = info.extent.width,
		.height = info.extent.height,
		.format = engine::ETNA_DEPTH_FORMAT,
		.sampleCount = sampleCountBits,
	}));

	Command cmd({.device = _device, .queue = engine::getUploadQueue()});

	cmd.begin();

	cmd.transitionToOptimalLayout(*m_drawImage);

	cmd.transitionToOptimalLayout(*m_depthImage);

	if (isMultiSampled())
		cmd.transitionToOptimalLayout(*m_resolvedImage);

	cmd.end();

	SubmitCmdInfo uploadCmdInfo{.command = cmd};

	_device.submitCommands({uploadCmdInfo}, nullptr);

	_device.waitIdle();
}

RenderTarget::~RenderTarget() {
	delete m_drawImage;
	delete m_depthImage;
	delete m_resolvedImage;
}
