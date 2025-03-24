#pragma once

#include "ignis/image.hpp"

namespace etna {

class RenderTarget {
	friend class Engine;

public:
	// usage(s) will be inferred
	struct CreateInfo {
		VkExtent2D extent;
		ignis::ColorFormat colorFormat;
		ignis::DepthFormat depthFormat;
		uint32_t samples{1};
	};

	RenderTarget(const CreateInfo&);

	~RenderTarget();

	auto getDrawImage() const { return m_drawImage; }

	auto getDepthImage() const { return m_depthImage; }

	auto getResolvedImage() const { return m_resolvedImage; }

	auto getExtent() const { return m_creationInfo.extent; }

protected:
	ignis::Image* m_drawImage{nullptr};
	ignis::Image* m_depthImage{nullptr};
	ignis::Image* m_resolvedImage{nullptr};

	CreateInfo m_creationInfo;
};

}  // namespace etna
