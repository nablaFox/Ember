#include "light.hpp"
#include "engine.hpp"

using namespace etna;

DirectionalLight::DirectionalLight(Vec3 direction, float intensity, Color color)
	: m_direction(direction.normalize()), m_intensity(intensity), m_color(color) {
	const DirectionalLightData lightData{
		.direction = m_direction,
		.intensity = m_intensity,
		.color = m_color,
	};

	m_buffer = _device.createSSBO(sizeof(DirectionalLightData), &lightData);
}

DirectionalLight::~DirectionalLight() {
	_device.destroyBuffer(m_buffer);
}
