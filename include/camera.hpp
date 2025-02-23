#pragma once

#include "types.hpp"

struct Camera {
	float fov = 90.f;
	float aspect = 1.77f;
	float near = 0.1f;
	float far = 100.f;

	WorldTransform transform{};

	void rotate(Rot rotation);

	void translate(Vec3 translation);

	Mat4 getViewMatrix();

	Mat4 getProjMatrix();

	Mat4 getViewProjMatrix() { return getProjMatrix() * getViewMatrix(); }
};
