#pragma once

#include <cstdint>
#include "math.hpp"

struct Rot {
	float yaw{0}, pitch{0}, roll{0};

	Mat4 getRotMatrix() const {
		Mat4 yawMat = {
			{cosf(yaw), 0, sinf(yaw), 0},
			{0, 1, 0, 0},
			{-sinf(yaw), 0, cosf(yaw), 0},
			{0, 0, 0, 1},
		};

		Mat4 pitchMat = {
			{1, 0, 0, 0},
			{0, cosf(pitch), -sinf(pitch), 0},
			{0, sinf(pitch), cosf(pitch), 0},
			{0, 0, 0, 1},
		};

		Mat4 rollMat = {
			{cosf(roll), -sinf(roll), 0, 0},
			{sinf(roll), cosf(roll), 0, 0},
			{0, 0, 1, 0},
			{0, 0, 0, 1},
		};

		return pitchMat * yawMat * rollMat;
	}
};

struct Color {
	float r, g, b, a;
};

struct Vertex {
	Vec3 position;
	float padding;
	Color color;
};

typedef uint32_t Index;

struct WorldTransform {
	float scale = 1;
	Rot rotation = {0, 0, 0};
	Vec3 translation = {0, 0, 0};

	Mat4 getWorldMatrix() const {
		return getTransMatrix() * getScaleMatrix() * getRotMatrix();
	}

	Mat4 getTransMatrix() const {
		return {
			{1, 0, 0, translation[0]},
			{0, 1, 0, translation[1]},
			{0, 0, 1, translation[2]},
			{0, 0, 0, 1},
		};
	}

	Mat4 getScaleMatrix() const {
		return {
			{scale, 0, 0, 0},
			{0, scale, 0, 0},
			{0, 0, scale, 0},
			{0, 0, 0, 1},
		};
	}

	Mat4 getRotMatrix() const { return rotation.getRotMatrix(); }
};

struct DirectionalLight {
	Vec3 direction;
	Color color;
};
