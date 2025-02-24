#pragma once

#include "types.hpp"

struct Camera {
	float fov = 90.f;
	float aspect = 1.77f;
	float near = 0.1f;
	float far = 100.f;

	Vec3 forward() const {
		Rot rot = transform.rotation;

		Vec3 res = {
			sinf(-rot.yaw) * cosf(rot.pitch),
			sinf(rot.pitch),
			cosf(-rot.yaw) * cosf(rot.pitch),
		};

		return res * -1;
	}

	Vec3 right() const { return forward().cross({0, 1, 0}).normalize(); }

	Vec3 up() const { return right().cross(forward()).normalize(); }

	WorldTransform transform{};

	void rotate(Rot rotation);

	void translate(Vec3 translation);

	Mat4 getViewMatrix();

	Mat4 getProjMatrix();

	Mat4 getViewProjMatrix() { return getProjMatrix() * getViewMatrix(); }
};
