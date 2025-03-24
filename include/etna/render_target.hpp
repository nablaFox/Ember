#pragma once

#include <optional>
#include "ignis/command.hpp"

namespace etna {

class RenderTarget {
	friend class Engine;

public:
	// usage(s) and load/store ops will be inferred
	struct CreateInfo {
		VkExtent2D extent;
		bool hasColor{true};
		bool hasDepth{true};
		uint32_t samples{1};
	};

	RenderTarget(const CreateInfo&);

	~RenderTarget();

	auto getDrawAttachment() const { return m_drawAttachment; }

	auto getDepthAttachment() const { return m_depthAttachment; }

	auto getExtent() const { return m_creationInfo.extent; }

protected:
	std::optional<ignis::DrawAttachment> m_drawAttachment;
	std::optional<ignis::DepthAttachment> m_depthAttachment;

	CreateInfo m_creationInfo;
};

}  // namespace etna
