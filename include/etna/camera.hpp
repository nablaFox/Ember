#pragma once

#include "transform.hpp"

namespace etna {

struct Viewport {
	float x{0};
	float y{0};
	float width{0};
	float height{0};
};

struct Camera {
	float fov{90.f};
	float aspect{1.77f};
	float near{0.1f};
	float far{100.f};

	Mat4 getProjMatrix() const {
		const float fovAngle = fov * M_PI / 180;

		const float top = near * tanf(fovAngle / 2);

		const float right = top * aspect;

		return {
			{near / right, 0, 0, 0},
			{0, -near / top, 0, 0},
			{0, 0, far / (near - far), near * far / (near - far)},
			{0, 0, -1, 0},
		};
	}

	Mat4 getViewProjMatrix(const Transform& transform) const {
		return getProjMatrix() * transform.getViewMatrix();
	}
};

}  // namespace etna
