#pragma once

#include "ignis/types.hpp"
#include "color.hpp"
#include "math.hpp"

namespace etna {

struct DirectionalLight {
	DirectionalLight(Vec3 direction, float intensity, Color color);
	~DirectionalLight();

	ignis::BufferId getDataBuffer() const { return m_buffer; }

	Vec3 getDirection() const { return m_direction; }
	float getIntensity() const { return m_intensity; }

private:
	Vec3 m_direction;
	float m_intensity;
	Color m_color;

	ignis::BufferId m_buffer{IGNIS_INVALID_BUFFER_ID};

	// TEMP: will contain also info for shadows (viewproj etc.)
	struct DirectionalLightData {
		Vec3 direction;
		float intensity;
		Color color;
	};
};

}  // namespace etna
