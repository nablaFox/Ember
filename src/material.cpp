#include "material.hpp"
#include "device.hpp"
#include "types.hpp"

using namespace ignis;
using namespace ember;

Material::Material(CreateInfo info) {
	auto device = EmberDevice::getDevice();

	if (info.polygonMode != VK_POLYGON_MODE_FILL &&
		!device->isFeatureEnabled("FillModeNonSolid")) {
		info.polygonMode = VK_POLYGON_MODE_FILL;
	}

	Pipeline::CreateInfo pipelineInfo{
		.device = device,
		.shaders = info.shaders,
		.colorFormat = EMBER_COLOR_FORMAT,
		.depthFormat = EMBER_DEPTH_FORMAT,
		.cullMode = VK_CULL_MODE_NONE,
		.polygonMode = info.polygonMode,
		.lineWidth = info.lineWidth,
		.sampleCount = EmberDevice::getSampleCount(),
		.sampleShadingEnable = device->isFeatureEnabled("SampleRateShading"),
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

	m_params = Buffer::createUBO(device, info.paramsSize, info.paramsData);

	// TEMP: in the future we may want to recycle freed handles
	static uint32_t handleCounter = 1;
	m_materialHandle = handleCounter++;

	device->registerUBO(*m_params, m_materialHandle);
}

Material::~Material() {
	delete m_pipeline;
}

void Material::updateParams(void* data) {
	m_params->writeData(data);
}
