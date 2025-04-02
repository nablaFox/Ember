#include "engine.hpp"
#include "render_target.hpp"

using namespace ignis;

namespace etna {

RenderTarget::RenderTarget(const CreateInfo& info) : m_creationInfo(info) {
	Device& device = engine::getDevice();

	const uint32_t maxSampleCount = engine::getMaxAllowedSampleCount();

	const uint32_t sampleCount = (info.samples == 0 || info.samples > maxSampleCount)
									 ? maxSampleCount
									 : info.samples;

	m_creationInfo.samples = sampleCount;

	const VkSampleCountFlagBits sampleCountBits =
		static_cast<VkSampleCountFlagBits>(sampleCount);

	m_drawImage = new Image(device.createDrawAttachmentImage({
		.width = info.extent.width,
		.height = info.extent.height,
		.format = engine::ETNA_COLOR_FORMAT,
		.sampleCount = sampleCountBits,
	}));

	if (isMultiSampled()) {
		m_resolvedImage = new Image(device.createDrawAttachmentImage({
			.width = info.extent.width,
			.height = info.extent.height,
			.format = engine::ETNA_COLOR_FORMAT,
			.sampleCount = VK_SAMPLE_COUNT_1_BIT,
		}));
	}

	if (info.hasDepth) {
		m_depthImage = new Image(device.createDepthAttachmentImage({
			.width = info.extent.width,
			.height = info.extent.height,
			.format = engine::ETNA_DEPTH_FORMAT,
			.sampleCount = sampleCountBits,
		}));
	}

	Command cmd({.device = device, .queue = engine::getUploadQueue()});

	cmd.begin();

	cmd.transitionToOptimalLayout(*m_drawImage);

	if (isMultiSampled())
		cmd.transitionToOptimalLayout(*m_resolvedImage);

	if (info.hasDepth)
		cmd.transitionToOptimalLayout(*m_depthImage);

	cmd.end();

	SubmitCmdInfo uploadCmdInfo{.command = cmd};

	device.submitCommands({uploadCmdInfo}, nullptr);

	device.waitIdle();
}

RenderTarget::~RenderTarget() {
	delete m_drawImage;
	delete m_depthImage;
	delete m_resolvedImage;
}

}  // namespace etna
