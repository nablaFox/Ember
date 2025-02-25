#pragma once

#include "types.hpp"

struct Camera {
	float fov = 90.f;
	float aspect = 1.77f;
	float near = 0.1f;
	float far = 100.f;

	Vec3 forward() const {
		Vec3 res = {
			sinf(-transform.yaw) * cosf(transform.pitch),
			sinf(transform.pitch),
			cosf(-transform.yaw) * cosf(transform.pitch),
		};

		return res * -1;
	}

	Vec3 right() const { return forward().cross({0, 1, 0}).normalize(); }

	Vec3 up() const { return right().cross(forward()).normalize(); }

	WorldTransform transform{};

	void rotate(float yaw, float pitch, float roll);

	void translate(Vec3 translation);

	Mat4 getViewMatrix() {
		Mat4 viewTranslation =
			WorldTransform::getTransMatrix(transform.position * -1);

		// CHECK shouldn't be transposed?
		Mat4 viewRotation = transform.getRotMatrix();

		return viewRotation * viewTranslation;
	};

	Mat4 getProjMatrix() {
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

	Mat4 getViewProjMatrix() { return getProjMatrix() * getViewMatrix(); }
};
