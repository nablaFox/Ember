#include "etna/engine.hpp"
#include "etna/render_target.hpp"

using namespace ignis;
using namespace etna;

RenderTarget::RenderTarget(const CreateInfo& info) : m_creationInfo(info) {
	const uint32_t sampleCount{engine::clampSampleCount(info.samples)};
	m_creationInfo.samples = sampleCount;

	m_drawImage = new Image(_device.createDrawAttachmentImage({
		.width = info.extent.width,
		.height = info.extent.height,
		.format = engine::COLOR_FORMAT,
		.sampleCount = static_cast<VkSampleCountFlagBits>(sampleCount),
	}));

	if (isMultiSampled()) {
		m_resolvedImage = new Image(_device.createDrawAttachmentImage({
			.width = info.extent.width,
			.height = info.extent.height,
			.format = engine::COLOR_FORMAT,
			.sampleCount = VK_SAMPLE_COUNT_1_BIT,
		}));
	}

	m_depthImage = new Image(_device.createDepthAttachmentImage({
		.width = info.extent.width,
		.height = info.extent.height,
		.format = engine::DEPTH_FORMAT,
		.sampleCount = static_cast<VkSampleCountFlagBits>(sampleCount),
	}));

	etna::engine::immediateSubmit([&](ignis::Command& cmd) {
		cmd.transitionToOptimalLayout(*m_drawImage);

		cmd.transitionToOptimalLayout(*m_depthImage);

		if (isMultiSampled())
			cmd.transitionToOptimalLayout(*m_resolvedImage);
	});
}

RenderTarget::~RenderTarget() {
	delete m_drawImage;
	delete m_depthImage;
	delete m_resolvedImage;
}
