#pragma once

#include <memory>
#include "ignis/types.hpp"
#include "ignis/pipeline.hpp"

namespace etna {

class _Material {
	friend class Engine;

public:
	struct CreateInfo {
		std::vector<std::string> shaders;
		size_t paramsSize{0};
		void* paramsData{nullptr};
		bool enableDepthTest{true};
		bool transparency{false};
		VkPolygonMode polygonMode{VK_POLYGON_MODE_FILL};
		float lineWidth{1.0f};
	};

	~_Material();

	void updateParams(const void* data) const;

	auto& getPipeline() const { return *m_pipeline; }

	auto getParamsUBO() const { return m_paramsUBO; }

#ifndef NDEBUG
	auto hasDepthTest() const { return depthTest; }
#endif

private:
	_Material(const CreateInfo&);

	BufferId m_paramsUBO{IGNIS_INVALID_BUFFER_ID};
	ignis::Pipeline* m_pipeline{nullptr};

#ifndef NDEBUG
	bool depthTest;
#endif

public:
	_Material(const _Material&) = delete;
	_Material(_Material&&) = delete;
	_Material& operator=(const _Material&) = delete;
	_Material& operator=(_Material&&) = delete;
};

using Material = std::shared_ptr<_Material>;

}  // namespace etna
