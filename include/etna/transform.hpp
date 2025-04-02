#pragma once

#include "math.hpp"

namespace etna {

struct Transform {
	Vec3 position{0, 0, 0};
	float yaw{0}, pitch{0}, roll{0};
	Vec3 scale{1, 1, 1};

	Mat4 getWorldMatrix() const {
		return getTransMatrix() * getScaleMatrix() * getRotMatrix();
	}

	Mat4 getTransMatrix() const { return getTransMatrix(position); }

	Mat4 getScaleMatrix() const { return getScaleMatrix(scale); }

	static Mat4 getScaleMatrix(Vec3 scale) {
		return {
			{scale[0], 0, 0, 0},
			{0, scale[1], 0, 0},
			{0, 0, scale[2], 0},
			{0, 0, 0, 1},
		};
	}

	static Mat4 getTransMatrix(Vec3 translation) {
		return {
			{1, 0, 0, translation[0]},
			{0, 1, 0, translation[1]},
			{0, 0, 1, translation[2]},
			{0, 0, 0, 1},
		};
	}

	Mat4 getYawMatrix() const {
		return {
			{cosf(yaw), 0, sinf(yaw), 0},
			{0, 1, 0, 0},
			{-sinf(yaw), 0, cosf(yaw), 0},
			{0, 0, 0, 1},
		};
	}

	Mat4 getPitchMatrix() const {
		return {
			{1, 0, 0, 0},
			{0, cosf(pitch), -sinf(pitch), 0},
			{0, sinf(pitch), cosf(pitch), 0},
			{0, 0, 0, 1},
		};
	}

	Mat4 getRollMatrix() const {
		return {
			{cosf(roll), -sinf(roll), 0, 0},
			{sinf(roll), cosf(roll), 0, 0},
			{0, 0, 1, 0},
			{0, 0, 0, 1},
		};
	}

	Mat4 getRotMatrix() const {
		return getPitchMatrix() * getYawMatrix() * getRollMatrix();
	}

	// PONDER: maybe add a getSubMatrix method to Mat
	Mat3 getRotationMat3() const {
		Mat4 rot4 = getRotMatrix();
		return {
			{rot4(0, 0), rot4(0, 1), rot4(0, 2)},
			{rot4(1, 0), rot4(1, 1), rot4(1, 2)},
			{rot4(2, 0), rot4(2, 1), rot4(2, 2)},
		};
	}

	Vec3 forward() const {
		Vec3 const res{
			sinf(-yaw) * cosf(pitch),
			sinf(pitch),
			cosf(-yaw) * cosf(pitch),
		};

		return res * -1;
	}

	Vec3 right() const { return forward().cross({0, 1, 0}).normalize(); }

	Vec3 up() const { return right().cross(forward()).normalize(); }
};

}  // namespace etna
