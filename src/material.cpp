#include "material.hpp"
#include "types.hpp"
#include "device.hpp"

Material::Material(std::vector<std::string> shaders,
				   uint32_t dataSize,
				   void* initialData) {
	auto device = EmberDevice::getDevice();

	m_pipeline = new Pipeline({
		.device = device,
		.shaders = std::move(shaders),
		.colorFormat = drawColorFormat,
		.depthFormat = depthAttachmentFormat,
	});

	if (dataSize == 0) {
		return;
	}

	m_params = Buffer::createUBO(device, dataSize, initialData);

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
