#pragma once

#include "ignis/ignis.hpp"

class Material {
public:
	virtual ~Material();

	struct CreateInfo {
		std::vector<std::string> shaders;
		VkPolygonMode polygonMode{VK_POLYGON_MODE_FILL};
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
		bool transparency{false};
	};

	MaterialTemplate(CreateInfo info) : m_info(info) {}

	Material create(T data) const {
		auto material = Material({
			m_info.shaders,
			m_info.polygonMode,
			m_info.transparency,
			sizeof(T),
			&data,
		});

		return material;
	}

	Material create(T data, CreateInfo overwriteInfo) const {
		auto material = Material({
			overwriteInfo.shaders.empty() ? m_info.shaders : overwriteInfo.shaders,
			overwriteInfo.polygonMode,
			overwriteInfo.transparency,
			sizeof(T),
			&data,
		});

		return material;
	}

private:
	CreateInfo m_info;
};
