#include "material.hpp"
#include "engine.hpp"

using namespace ignis;

namespace etna {

MaterialTemplate::MaterialTemplate(const CreateInfo& info)
	: m_paramsSize(info.paramsSize) {
	Device& device = Engine::getDevice();

	PipelineCreateInfo pipelineInfo{
		.device = &device,
		.shaders = std::move(info.shaders),
		.colorFormat = Engine::ETNA_COLOR_FORMAT,
		.cullMode = VK_CULL_MODE_NONE,
		.polygonMode = info.polygonMode,
		.lineWidth = info.lineWidth,
		.sampleCount = device.getMaxSampleCount(),
		.sampleShadingEnable = device.isFeatureEnabled("SampleRateShading"),
	};

	if (info.polygonMode != VK_POLYGON_MODE_FILL &&
		!device.isFeatureEnabled("FillModeNonSolid")) {
		pipelineInfo.polygonMode = VK_POLYGON_MODE_FILL;
	}

	if (info.enableDepth) {
		pipelineInfo.depthFormat = Engine::ETNA_DEPTH_FORMAT;
		pipelineInfo.enableDepthTest = true;
		pipelineInfo.enableDepthWrite = true;
	}

	if (info.transparency) {
		pipelineInfo.enableDepthWrite = false;
		pipelineInfo.blendEnable = true;
		pipelineInfo.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		pipelineInfo.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		pipelineInfo.colorBlendOp = VK_BLEND_OP_ADD;
	}

	m_pipeline = new Pipeline(pipelineInfo);

#ifndef NDEBUG
	hasDepth = info.enableDepth;
#endif
}

MaterialTemplate::~MaterialTemplate() {
	delete m_pipeline;
}

MaterialTemplateHandle MaterialTemplate::create(const CreateInfo& info) {
	return std::shared_ptr<MaterialTemplate>(new MaterialTemplate(info));
}

Material::Material(const CreateInfo& info)
	: m_materialTemplate(info.templateHandle) {
	size_t paramsSize = m_materialTemplate->getParamsSize();

	if (!paramsSize) {
		return;
	}

	m_paramsUBO = Engine::getDevice().createUBO(paramsSize, info.params);
}

Material::Material(const CreateInfo2& info) {
	m_materialTemplate = MaterialTemplate::create({
		.shaders = info.shaders,
		.paramsSize = info.paramsSize,
		.enableDepth = info.enableDepth,
		.transparency = info.transparency,
		.polygonMode = info.polygonMode,
		.lineWidth = info.lineWidth,
	});

	if (!info.paramsSize) {
		return;
	}

	m_paramsUBO = Engine::getDevice().createUBO(info.paramsSize, info.paramsData);
}

MaterialHandle Material::create(const CreateInfo& info) {
	return std::shared_ptr<Material>(new Material(info));
}

MaterialHandle Material::create(const CreateInfo2& info) {
	return std::shared_ptr<Material>(new Material(info));
}

Material::~Material() {
	if (m_paramsUBO != IGNIS_INVALID_BUFFER_ID)
		Engine::getDevice().destroyBuffer(m_paramsUBO);
}

}  // namespace etna
