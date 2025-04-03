#pragma once

#include "etna/forward_renderer.hpp"
#include "etna/default_materials.hpp"
#include "etna/default_primitives.hpp"
#include "etna/scene.hpp"
#include "etna/window.hpp"
#include "etna/model.hpp"

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

	if (window.isMouseCaptured()) {
		camera.updateTransform(transform);
	}
}

struct FloorCreateInfo {
	Color color{};
	float gridSize{1};
	float lineThickness{0.02};
	float height{0};
	float floorScale{1000};
};

inline ModelRoot createFloor(const FloorCreateInfo& info) {
	MeshHandle quad = engine::getQuad();

	MaterialHandle mainGridMaterial = engine::createTransparentGridMaterial({
		.color = INVISIBLE,
		.gridColor = info.color,
		.gridSpacing = info.gridSize / info.floorScale,
		.thickness = info.lineThickness / info.floorScale,
	});

	MaterialHandle subGridMaterial = engine::createTransparentGridMaterial({
		.color = INVISIBLE,
		.gridColor = info.color * 0.8,
		.gridSpacing = info.gridSize / (info.floorScale * 2.f),
		.thickness = info.lineThickness / info.floorScale,
	});

	Transform transform{
		.position = {0, info.height, 0},
		.pitch = -M_PI / 2,
		.scale = Vec3(info.floorScale),
	};

	ModelRoot floor = Model::createRoot("Floor", {.transform = transform});

	floor->add("MainGrid", {.mesh = quad, .material = mainGridMaterial});

	floor->add("SubGrid", {.mesh = quad, .material = subGridMaterial});

	return floor;
}

struct OutlinedBrickCreateInfo {
	Color color{WHITE};
	Color outlineColor{};
	float outlineThickness{0.01};
	float width{1};
	float height{1};
	float depth{1};
};

inline ModelRoot createOutlinedBrick(const OutlinedBrickCreateInfo& info) {
	MeshHandle cube = engine::getUVCube();

	MaterialHandle material = engine::createBrickOutlinedMaterial({
		.color = info.color,
		.outline = info.outlineColor,
		.thickness = info.outlineThickness,
	});

	Transform transform{
		.scale = Vec3(info.width, info.height, info.depth),
	};

	return Model::createRoot(
		"OutlinedBrick",
		{.mesh = cube, .material = material, .transform = transform});
}
