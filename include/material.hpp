#pragma once

#include "ignis/ignis.hpp"

using namespace ignis;

class Material {
public:
	virtual ~Material();

	// will auto generate an ubo handle
	Material(const char* fragShader,
			 const char* vertShader,
			 uint32_t dataSize = 0,
			 void* initialData = nullptr);

	auto getHandle() const { return m_materialHandle; }

	auto& getPipeline() const { return *m_pipeline; }

	auto updateData(void* data) -> void;

private:
	Pipeline* m_pipeline{nullptr};
	Buffer* m_ubo{nullptr};
	uint32_t m_materialHandle{0};
};

template <typename T>
class MaterialTemplate : public Material {
public:
	MaterialTemplate(const char* fragShader, const char* vertShader, T initialData)
		: Material(fragShader, vertShader, sizeof(T), &initialData) {}

	auto updateData(T data) -> void { Material::updateData(&data); }
};

inline Material defaultMaterial{
	"default.frag.spv",
	"default.vert.spv",
};
