#include "engine.hpp"
#include "render_target.hpp"

using namespace ignis;

namespace etna {

RenderTarget::RenderTarget(const CreateInfo& info) : m_creationInfo(info) {
	Device& device = Engine::getDevice();

	assert(info.samples > 0 && "Samples must be greater than 0");

	assert(info.samples <= device.getMaxSampleCount() &&
		   "Samples must be less than or equal to the maximum sample count");

	m_drawImage = new Image(device.createDrawAttachmentImage({
		.width = info.extent.width,
		.height = info.extent.height,
		.format = Engine::ETNA_COLOR_FORMAT,
		.sampleCount = static_cast<VkSampleCountFlagBits>(info.samples),
	}));

	if (isMultiSampled()) {
		m_resolvedImage = new Image(device.createDrawAttachmentImage({
			.width = info.extent.width,
			.height = info.extent.height,
			.format = Engine::ETNA_COLOR_FORMAT,
			.sampleCount = VK_SAMPLE_COUNT_1_BIT,
		}));
	}

	if (info.hasDepth) {
		m_depthImage = new Image(device.createDepthAttachmentImage({
			.width = info.extent.width,
			.height = info.extent.height,
			.format = Engine::ETNA_DEPTH_FORMAT,
			.sampleCount = static_cast<VkSampleCountFlagBits>(info.samples),
		}));
	}

	Command cmd({.device = device, .queue = Engine::getUploadQueue()});

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
