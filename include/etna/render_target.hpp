#pragma once

#include "ignis/command.hpp"

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

	auto getDrawImage() const { return m_drawImage; }

	auto getResolvedImage() const { return m_resolvedImage; }

	auto getDepthImage() const { return m_depthImage; }

	auto getExtent() const { return m_creationInfo.extent; }

	bool isMultiSampled() const { return m_creationInfo.samples > 1; }

	CreateInfo getCreationInfo() const { return m_creationInfo; }

protected:
	ignis::Image* m_drawImage;
	ignis::Image* m_depthImage;
	ignis::Image* m_resolvedImage;

	CreateInfo m_creationInfo;
};

}  // namespace etna
