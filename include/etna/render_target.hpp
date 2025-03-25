#pragma once

#include <optional>
#include "ignis/command.hpp"
#include "image.hpp"

namespace etna {

class RenderTarget {
public:
	struct CreateInfo {
		VkExtent2D extent;
		bool hasColor{true};
		bool hasDepth{true};
		uint32_t samples{1};
	};

	RenderTarget(const CreateInfo&);

	~RenderTarget();

	// PONDER: maybe is the resolved image that is useful to be returned
	std::optional<ignis::Image*> getDrawImage() const {
		if (!m_drawAttachment)
			return std::nullopt;

		return m_drawAttachment->drawImage;
	}

	std::optional<ignis::Image*> getDepthImage() const {
		if (!m_depthAttachment)
			return std::nullopt;

		return m_depthAttachment->depthImage;
	}

	auto getExtent() const { return m_creationInfo.extent; }

	bool isMultiSampled() const { return m_creationInfo.samples > 1; }

protected:
	std::optional<ignis::DrawAttachment> m_drawAttachment;
	std::optional<ignis::DepthAttachment> m_depthAttachment;
	ignis::Image* m_resolvedImage;

	CreateInfo m_creationInfo;
};

}  // namespace etna
