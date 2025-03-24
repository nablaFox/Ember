#include "material.hpp"
#include "engine.hpp"

using namespace ignis;

namespace etna {

_Material::_Material(const Engine* engine, const CreateInfo& info)
	: m_device(engine->getDevice()) {
	CreateInfo actualInfo = info;

	if (info.polygonMode != VK_POLYGON_MODE_FILL &&
		!m_device.isFeatureEnabled("FillModeNonSolid")) {
		actualInfo.polygonMode = VK_POLYGON_MODE_FILL;
	}

	PipelineCreateInfo pipelineInfo{
		.device = &m_device,
		.shaders = info.shaders,
		.colorFormat = Engine::ETNA_COLOR_FORMAT,
		.depthFormat = Engine::ETNA_DEPTH_FORMAT,
		.cullMode = VK_CULL_MODE_NONE,
		.polygonMode = info.polygonMode,
		.lineWidth = info.lineWidth,
		.sampleCount = m_device.getMaxSampleCount(),
		.sampleShadingEnable = m_device.isFeatureEnabled("SampleRateShading"),
		.enableDepthTest = true,
		.enableDepthWrite = true,
	};

	if (info.transparency) {
		pipelineInfo.enableDepthWrite = false;
		pipelineInfo.blendEnable = true;
		pipelineInfo.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		pipelineInfo.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		pipelineInfo.colorBlendOp = VK_BLEND_OP_ADD;
	}

	m_pipeline = new Pipeline(pipelineInfo);

	if (info.paramsSize == 0) {
		return;
	}

	m_paramsUBO = m_device.createUBO(info.paramsSize, info.paramsData);
}

_Material::~_Material() {
	delete m_pipeline;
	m_device.destroyBuffer(m_paramsUBO);
}

}  // namespace etna
