#include "light.hpp"
#include "engine.hpp"

using namespace etna;

DirectionalLight::DirectionalLight(const CreateInfo& info)
	: m_direction(info.direction),
	  m_intensity(info.intensity),
	  m_color(info.color),
	  m_name(info.name) {
	const DirectionalLightData lightData{
		.direction = m_direction,
		.intensity = m_intensity,
		.color = m_color,
	};

	m_buffer = _device.createUBO(sizeof(DirectionalLightData), &lightData);
}

DirectionalLight::~DirectionalLight() {
	_device.destroyBuffer(m_buffer);
}

void DirectionalLight::update(const CreateInfo& info) {
	m_direction = info.direction;
	m_intensity = info.intensity;
	m_color = info.color;
	m_name = info.name;

	const DirectionalLightData lightData{
		.direction = m_direction,
		.intensity = m_intensity,
		.color = m_color,
	};

	_device.updateBuffer(m_buffer, &lightData);
}

void DirectionalLight::updateDirection(const Vec3& direction) {
	update({
		.name = m_name,
		.direction = direction,
		.intensity = m_intensity,
		.color = m_color,
	});
}

void DirectionalLight::updateIntensity(float intensity) {
	update({
		.name = m_name,
		.direction = m_direction,
		.intensity = intensity,
		.color = m_color,
	});
}

void DirectionalLight::updateColor(const Color& color) {
	update({
		.name = m_name,
		.direction = m_direction,
		.intensity = m_intensity,
		.color = color,
	});
}

void DirectionalLight::translate(const Vec3& translation) {
	m_direction += translation;
	updateDirection(m_direction);
}
