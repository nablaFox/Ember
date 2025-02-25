#pragma once

#include "ignis/ignis.hpp"

using namespace ignis;

class Material {
public:
	virtual ~Material();

	struct CreateInfo {
		std::vector<std::string> shaders;
		VkPolygonMode polygonMode{VK_POLYGON_MODE_FILL};
		bool transparent{false};
		uint32_t paramsSize{0};
		const void* paramsData{nullptr};
	};

	Material(CreateInfo);

	auto getHandle() const { return m_materialHandle; }

	auto& getPipeline() const { return *m_pipeline; }

	auto updateParams(void* data) -> void;

private:
	Pipeline* m_pipeline{nullptr};
	Buffer* m_params{nullptr};
	uint32_t m_materialHandle{0};
};

template <typename T>
class MaterialTemplate {
public:
	struct CreateInfo {
		std::vector<std::string> shaders;
		VkPolygonMode polygonMode{VK_POLYGON_MODE_FILL};
		bool transparent{false};
	};

	MaterialTemplate(CreateInfo info) : m_info(info) {}

	Material create(T data) {
		auto material = Material({
			m_info.shaders,
			m_info.polygonMode,
			m_info.transparent,
			sizeof(T),
			&data,
		});

		return material;
	}

private:
	CreateInfo m_info;
};

inline Material defaultMaterial({
	.shaders = {"ember/default.frag.spv", "ember/default.vert.spv"},
});
