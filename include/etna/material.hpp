#pragma once

#include <memory>
#include "ignis/types.hpp"
#include "ignis/pipeline.hpp"

namespace etna {

class _Material {
public:
	struct CreateInfo {
		std::vector<std::string> shaders;
		size_t paramsSize{0};
		void* paramsData{nullptr};
		bool enableDepth{true};
		bool transparency{false};
		VkPolygonMode polygonMode{VK_POLYGON_MODE_FILL};
		float lineWidth{1.0f};
	};

	_Material(const CreateInfo&);

	~_Material();

	void updateParams(const void* data) const;

	auto& getPipeline() const { return *m_pipeline; }

	auto getParamsUBO() const { return m_paramsUBO; }

#ifndef NDEBUG
	auto hasDepthTest() const { return m_hasDepth; }
#endif

private:
	ignis::BufferId m_paramsUBO{IGNIS_INVALID_BUFFER_ID};
	ignis::Pipeline* m_pipeline{nullptr};

#ifndef NDEBUG
	bool m_hasDepth;
#endif

public:
	_Material(const _Material&) = delete;
	_Material(_Material&&) = delete;
	_Material& operator=(const _Material&) = delete;
	_Material& operator=(_Material&&) = delete;
};

using Material = std::shared_ptr<_Material>;

}  // namespace etna
