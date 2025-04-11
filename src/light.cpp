#include "etna/light.hpp"
#include "etna/engine.hpp"

using namespace etna;

DirectionalLight::DirectionalLight(const CreateInfo& info) : m_info(info) {
	const DirectionalLightData lightData{
		.direction = info.direction,
		.intensity = info.intensity,
		.color = info.color,
	};

	m_buffer = _device.createUBO(sizeof(DirectionalLightData), &lightData);
}

DirectionalLight::~DirectionalLight() {
	_device.destroyBuffer(m_buffer);
}

void DirectionalLight::update(const CreateInfo& info) {
	const DirectionalLightData lightData{
		.direction = info.direction,
		.intensity = info.intensity,
		.color = info.color,
	};

	_device.updateBuffer(m_buffer, &lightData);
}

void DirectionalLight::updateDirection(const Vec3& direction) {
	update({
		.name = getName(),
		.direction = direction,
		.intensity = getIntensity(),
		.color = getColor(),
	});
}

void DirectionalLight::updateIntensity(float intensity) {
	update({
		.name = getName(),
		.direction = getDirection(),
		.intensity = intensity,
		.color = getColor(),
	});
}

void DirectionalLight::updateColor(const Color& color) {
	update({
		.name = getName(),
		.direction = getDirection(),
		.intensity = getIntensity(),
		.color = color,
	});
}
