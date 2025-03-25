#include "material.hpp"
#include "device.hpp"
#include "engine.hpp"

using namespace ignis;

namespace etna {

_Material::_Material(const CreateInfo& info) {
	CreateInfo actualInfo = info;
	Device& device = Engine::getDevice();

	if (info.polygonMode != VK_POLYGON_MODE_FILL &&
		!device.isFeatureEnabled("FillModeNonSolid")) {
		actualInfo.polygonMode = VK_POLYGON_MODE_FILL;
	}

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

#ifndef NDEBUG
	depthTest = info.enableDepthTest;
#endif

	if (info.enableDepthTest) {
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

	if (info.paramsSize == 0) {
		return;
	}

	m_paramsUBO = device.createUBO(info.paramsSize, info.paramsData);
}

_Material::~_Material() {
	delete m_pipeline;

	Engine::getDevice().destroyBuffer(m_paramsUBO);
}

}  // namespace etna
