#pragma once

#include <vector>
#include <string>
#include "ignis/buffer.hpp"
#include "ignis/pipeline.hpp"

namespace ember {

class Material {
public:
	virtual ~Material();

	struct CreateInfo {
		std::vector<std::string> shaders;
		VkPolygonMode polygonMode{VK_POLYGON_MODE_FILL};
		float lineWidth{1.0f};
		bool transparency{false};
		uint32_t paramsSize{0};
		const void* paramsData{nullptr};
	};

	Material(CreateInfo);

	auto getHandle() const { return m_materialHandle; }

	auto& getPipeline() const { return *m_pipeline; }

	auto updateParams(void* data) -> void;

private:
	ignis::Pipeline* m_pipeline{nullptr};
	ignis::Buffer* m_params{nullptr};
	uint32_t m_materialHandle{0};
};

template <typename T>
class MaterialTemplate {
public:
	struct CreateInfo {
		std::vector<std::string> shaders;
		VkPolygonMode polygonMode{VK_POLYGON_MODE_FILL};
		float lineWidth{1.0f};
		bool transparency{false};
	};

	MaterialTemplate(CreateInfo info) : m_info(info) {}

	Material create(T data) const {
		auto material = Material({
			.shaders = m_info.shaders,
			.polygonMode = m_info.polygonMode,
			.lineWidth = m_info.lineWidth,
			.transparency = m_info.transparency,
			.paramsSize = sizeof(T),
			.paramsData = &data,
		});

		return material;
	}

	Material create(T data, CreateInfo overwriteInfo) const {
		auto material = Material({
			.shaders = m_info.shaders,
			.polygonMode = overwriteInfo.polygonMode,
			.transparency = overwriteInfo.transparency,
			.paramsSize = sizeof(T),
			.paramsData = &data,
		});

		return material;
	}

private:
	CreateInfo m_info;
};

};	// namespace ember
