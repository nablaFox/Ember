#pragma once

#include "ignis/ignis.hpp"

using namespace ignis;

class Material {
public:
	virtual ~Material();

	// will auto generate an ubo handle
	Material(std::vector<std::string> shaders,
			 uint32_t dataSize = 0,
			 void* initialData = nullptr);

	auto getHandle() const { return m_materialHandle; }

	auto& getPipeline() const { return *m_pipeline; }

	auto updateParams(void* data) -> void;

private:
	Pipeline* m_pipeline{nullptr};
	Buffer* m_params{nullptr};
	uint32_t m_materialHandle{0};
};

template <typename T>
class MaterialTemplate : public Material {
public:
	MaterialTemplate(std::vector<std::string> shaders, T initialData)
		: Material(std::move(shaders), sizeof(T), &initialData) {}

	auto updateParams(T data) -> void { Material::updateParams(&data); }
};

inline Material defaultMaterial{
	{
		"default.frag.spv",
		"default.vert.spv",
	},
};
