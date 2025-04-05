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
