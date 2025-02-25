#include "material.hpp"
#include "types.hpp"
#include "device.hpp"

Material::Material(CreateInfo info) {
	auto device = EmberDevice::getDevice();

	Pipeline::CreateInfo pipelineInfo{
		.device = device,
		.shaders = info.shaders,
		.colorFormat = drawAttachmentColorFormat,
		.depthFormat = depthAttachmentFormat,
		.cullMode = VK_CULL_MODE_NONE,
		.polygonMode = info.polygonMode,
		.sampleCount = EmberDevice::getSampleCount(),
		.sampleShadingEnable = true,  // TEMP
		.enableDepthTest = true,
		.enableDepthWrite = true,
	};

	if (info.transparent) {
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
