#pragma once

#include "engine.hpp"
#include "scene.hpp"
#include "window.hpp"
#include "model.hpp"

using namespace etna;

struct FirstPersonMovementOpts {
	bool flyAround{true};
	float deltaTime{1 / 60.f};
	float cameraSpeed{6.f};
	float sensitivity{0.001f};
};

inline void updateFirstPersonCamera(CameraNode& camera,
									Window& window,
									FirstPersonMovementOpts opts = {}) {
	Transform transform = camera.getTransform();

	transform.yaw += window.mouseDeltaX() * opts.sensitivity;
	transform.pitch += window.mouseDeltaY() * opts.sensitivity;

	if (transform.pitch > M_PI / 2) {
		transform.pitch = M_PI / 2;
	} else if (transform.pitch < -M_PI / 2) {
		transform.pitch = -M_PI / 2;
	}

	Vec3& position = transform.position;
	float yaw = -transform.yaw;

	const Vec3 playerForward =
		opts.flyAround ? transform.forward() : Vec3{sinf(yaw), 0, cosf(yaw)} * -1;

	const Vec3 playerRight =
		opts.flyAround ? transform.right() : Vec3{cosf(yaw), 0, -sinf(yaw)};

	const Vec3 up = opts.flyAround ? transform.up() : Vec3{0, 0, 0};

	if (window.isKeyPressed(GLFW_KEY_0)) {
		position = {0, 1, 0};
	}

	if (window.isKeyPressed(GLFW_KEY_W)) {
		position += playerForward * opts.cameraSpeed * opts.deltaTime;
	}

	if (window.isKeyPressed(GLFW_KEY_S)) {
		position -= playerForward * opts.cameraSpeed * opts.deltaTime;
	}

	if (window.isKeyPressed(GLFW_KEY_D)) {
		position += playerRight * opts.cameraSpeed * opts.deltaTime;
	}

	if (window.isKeyPressed(GLFW_KEY_A)) {
		position -= playerRight * opts.cameraSpeed * opts.deltaTime;
	}

	if (window.isKeyPressed(GLFW_KEY_SPACE)) {
		position += up * opts.cameraSpeed * opts.deltaTime;
	}

	if (window.isKeyPressed(GLFW_KEY_LEFT_SHIFT)) {
		position -= up * opts.cameraSpeed * opts.deltaTime;
	}

	if (window.isKeyPressed(GLFW_KEY_ESCAPE)) {
		window.setCaptureMouse(false);
	}

	if (window.isKeyPressed(GLFW_KEY_ENTER)) {
		window.setCaptureMouse(true);
	}

	camera.updateTransform(transform);
}

struct FloorCreateInfo {
	Color color{};
	float gridSize{1};
	float lineThickness{0.02};
	float height{0};
	float floorScale{1000};
};

inline Model createFloor(const FloorCreateInfo& info) {
	MeshHandle quad = Engine::createQuad(INVISIBLE);

	MaterialHandle mainGridMaterial = Engine::createTransparentGridMaterial({
		.color = info.color,
		.gridSpacing = info.gridSize / info.floorScale,
		.thickness = info.lineThickness / info.floorScale,
	});

	MaterialHandle subGridMaterial = Engine::createTransparentGridMaterial({
		.color = info.color * 0.8,
		.gridSpacing = info.gridSize / (info.floorScale * 2.f),
		.thickness = info.lineThickness / info.floorScale,
	});

	Transform mainGridTransform{
		.scale = info.floorScale,
		.pitch = -M_PI / 2,
		.position = {0, info.height, 0},
	};

	Transform subGridTransform{};

	Model floor(quad, mainGridMaterial, mainGridTransform);

	floor.getRoot().addSubMesh("SubGrid", quad, subGridMaterial, subGridTransform);

	return floor;
}
