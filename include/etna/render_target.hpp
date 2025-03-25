#pragma once

#include "ignis/command.hpp"
#include "image.hpp"

namespace etna {

class RenderTarget {
public:
	struct CreateInfo {
		VkExtent2D extent;
		// TODO: bool hasColor{true};
		bool hasDepth{true};
		uint32_t samples{1};
	};

	RenderTarget(const CreateInfo&);

	~RenderTarget();

	// PONDER: maybe is the resolved image that is useful to be returned
	auto getDrawImage() const { return m_drawAttachment.drawImage; }

	ignis::Image* getDepthImage() const {
		if (!m_depthAttachment)
			return nullptr;

		return m_depthAttachment->depthImage;
	}

	auto& getDrawAttachment() const { return m_drawAttachment; }

	auto getDepthAttachment() const { return m_depthAttachment; }

	auto getExtent() const { return m_creationInfo.extent; }

	bool isMultiSampled() const { return m_creationInfo.samples > 1; }

	CreateInfo getCreationInfo() const { return m_creationInfo; }

protected:
	ignis::DrawAttachment m_drawAttachment;
	ignis::DepthAttachment* m_depthAttachment;
	ignis::Image* m_resolvedImage;

	CreateInfo m_creationInfo;
};

}  // namespace etna
