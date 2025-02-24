#include <cmath>
#include "window.hpp"
#include "renderer.hpp"
#include "shapes.hpp"
#include "device.hpp"

constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 600;

auto updateCamera(Camera& camera, const Window& window) -> void {
	static float previousMouseX = window.getMouseX();
	static float previousMouseY = window.getMouseY();

	float deltaX = window.getMouseX() - previousMouseX;
	float deltaY = window.getMouseY() - previousMouseY;

	previousMouseX = window.getMouseX();
	previousMouseY = window.getMouseY();

	constexpr float sensitivity = 0.005f;

	camera.rotate({
		.yaw = deltaX * sensitivity,
		.pitch = deltaY * sensitivity,
	});

	constexpr float cameraSpeed = 0.1f;

	Vec3 translation{};

	if (window.isKeyPressed(GLFW_KEY_W)) {
		translation[0] -= cameraSpeed;
	}

	if (window.isKeyPressed(GLFW_KEY_S)) {
		translation[0] += cameraSpeed;
	}

	if (window.isKeyPressed(GLFW_KEY_D)) {
		translation[1] += cameraSpeed;
	}

	if (window.isKeyPressed(GLFW_KEY_A)) {
		translation[1] -= cameraSpeed;
	}

	if (window.isKeyPressed(GLFW_KEY_SPACE)) {
		translation[2] += cameraSpeed;
	}

	if (window.isKeyPressed(GLFW_KEY_LEFT_SHIFT)) {
		translation[2] -= cameraSpeed;
	}

	camera.translate(translation);
}

auto main(int argc, char* argv[]) -> int {
	Window window("Ember", WINDOW_WIDTH, WINDOW_HEIGHT);
	Renderer renderer(window);

	Camera mainCamera;
	DirectionalLight sun;

	mainCamera.fov = 60.f;
	mainCamera.aspect = (float)WINDOW_WIDTH / WINDOW_HEIGHT;

	Cube cube(1, {.b = 1});

	struct TestMaterialData {
		float r, g, b;
	};

	MaterialTemplate<TestMaterialData> testMaterial(
		"floor.frag.spv", "default.vert.spv", {.r = 0.f, .g = 0.f, .b = 0.f});

	cube.setMaterial(&testMaterial);

	while (!window.shouldClose()) {
		updateCamera(mainCamera, window);

		renderer.beginScene(mainCamera, sun);

		if (window.isKeyPressed(GLFW_KEY_R)) {
			testMaterial.updateData({.r = 1.f, .g = 0.f, .b = 0.f});
		}

		if (window.isKeyPressed(GLFW_KEY_G)) {
			testMaterial.updateData({.r = 0.f, .g = 1.f, .b = 0.f});
		}

		if (window.isKeyPressed(GLFW_KEY_B)) {
			testMaterial.updateData({.r = 0.f, .g = 0.f, .b = 1.f});
		}

		// white
		if (window.isKeyPressed(GLFW_KEY_W)) {
			testMaterial.updateData({.r = 1.f, .g = 1.f, .b = 1.f});
		}

		WorldTransform cubeTransform{
			.rotation = {.yaw = M_PI / 4},
			.translation = {1.5f, -0.5f, -3.f},
		};

		renderer.draw(cube, cubeTransform);

		renderer.endScene();
	}
}
