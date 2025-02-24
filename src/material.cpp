#include "material.hpp"
#include <cassert>
#include "types.hpp"
#include "device.hpp"

Material::Material(const char* fragShader,
				   const char* vertShader,
				   uint32_t dataSize,
				   void* initialData) {
	auto device = EmberDevice::getDevice();

	m_pipeline = new Pipeline({
		.device = device,
		.shaders = {vertShader, fragShader},
		.colorFormat = drawColorFormat,
		.depthFormat = depthAttachmentFormat,
	});

	if (dataSize == 0) {
		return;
	}

	m_ubo = Buffer::createUBO(device, dataSize, initialData);

	// TEMP generate handle
	uint32_t handle = 0;

	device->registerUBO(*m_ubo, handle);
}

Material::~Material() {
	delete m_pipeline;
}

void Material::updateData(void* data) {
	m_ubo->writeData(data);
}
