#include "engine.hpp"
#include "render_target.hpp"

using namespace ignis;

namespace etna {

RenderTarget::RenderTarget(const CreateInfo& info) : m_creationInfo(info) {
	Device& device = Engine::getDevice();

	assert(info.samples > 0 && "Samples must be greater than 0");

	assert(info.samples <= device.getMaxSampleCount() &&
		   "Samples must be less than or equal to the maximum sample count");

	Image* drawImage = new Image(device.createDrawAttachmentImage({
		.width = info.extent.width,
		.height = info.extent.height,
		.format = ETNA_COLOR_FORMAT,
		.sampleCount = static_cast<VkSampleCountFlagBits>(info.samples),
	}));

	m_drawAttachment = {
		.drawImage = drawImage,
		.loadAction = VK_ATTACHMENT_LOAD_OP_CLEAR,
		.storeAction = VK_ATTACHMENT_STORE_OP_STORE,
		.clearColor = info.clearColor,
	};

	if (isMultiSampled()) {
		m_resolvedImage = new Image(device.createDrawAttachmentImage({
			.width = info.extent.width,
			.height = info.extent.height,
			.format = ETNA_COLOR_FORMAT,
			.sampleCount = VK_SAMPLE_COUNT_1_BIT,
		}));
	}

	if (!info.hasDepth)
		return;

	Image* depthImage = new Image(device.createDepthAttachmentImage({
		.width = info.extent.width,
		.height = info.extent.height,
		.format = ETNA_DEPTH_FORMAT,
		.sampleCount = static_cast<VkSampleCountFlagBits>(info.samples),
	}));

	m_depthAttachment = new DepthAttachment{
		.depthImage = depthImage,
		.loadAction = VK_ATTACHMENT_LOAD_OP_CLEAR,
		.storeAction = VK_ATTACHMENT_STORE_OP_DONT_CARE,
	};
}

RenderTarget::~RenderTarget() {
	delete m_drawAttachment.drawImage;

	delete m_resolvedImage;

	if (m_depthAttachment)
		delete m_depthAttachment->depthImage;
}

}  // namespace etna
