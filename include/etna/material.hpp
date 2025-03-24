#pragma once

#include <memory>
#include "ignis/types.hpp"
#include "ignis/pipeline.hpp"

namespace etna {

class Engine;

class _Material {
public:
	struct CreateInfo {
		std::vector<std::string> shaders;
		size_t sizeParams{0};
		bool transparency{false};
		VkPolygonMode polygonMode{VK_POLYGON_MODE_FILL};
		float lineWidth{1.0f};
	};

	~_Material();

private:
	_Material(const Engine&, const CreateInfo&);

	const Engine& engine;
	BufferId m_paramsUBO;
	ignis::Pipeline m_pipeline;

public:
	_Material(const _Material&) = delete;
	_Material(_Material&&) = delete;
	_Material& operator=(const _Material&) = delete;
	_Material& operator=(_Material&&) = delete;
};

using Material = std::shared_ptr<_Material>;

}  // namespace etna
