#pragma once

#include "ignis/command.hpp"

namespace etna {

class RenderTarget {
public:
	struct CreateInfo {
		VkExtent2D extent;
		uint32_t samples{0};
	};

	RenderTarget(const CreateInfo&);

	~RenderTarget();

	auto getDrawImage() const { return m_drawImage; }

	auto getResolvedImage() const { return m_resolvedImage; }

	auto getDepthImage() const { return m_depthImage; }

	auto getExtent() const { return m_creationInfo.extent; }

	bool isMultiSampled() const { return m_creationInfo.samples > 1; }

	float getAspect() const {
		return m_creationInfo.extent.width /
			   static_cast<float>(m_creationInfo.extent.height);
	}

	CreateInfo getCreationInfo() const { return m_creationInfo; }

protected:
	ignis::Image* m_drawImage{nullptr};
	ignis::Image* m_depthImage{nullptr};
	ignis::Image* m_resolvedImage{nullptr};

	CreateInfo m_creationInfo;
};

}  // namespace etna
