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
	float fov{70.f};
	float near{0.1f};
	float far{100.f};

	Mat4 getProjMatrix(float aspect) const {
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

	Mat4 getViewMatrix(const Mat4& M) const {
		const Mat4 R{
			{M(0, 0), M(0, 1), M(0, 2), 0},
			{M(1, 0), M(1, 1), M(1, 2), 0},
			{M(2, 0), M(2, 1), M(2, 2), 0},
			{0, 0, 0, 1},
		};

		const Vec3 translation{M(0, 3), M(1, 3), M(2, 3)};

		const Mat4 T = Transform::getTransMatrix(translation * -1);

		return R * T;
	}
};

}  // namespace etna
