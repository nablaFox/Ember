#include <cmath>
#include "window.hpp"
#include "renderer.hpp"
#include "shapes.hpp"
#include "device.hpp"

constexpr int WINDOW_WIDTH = 1920;
constexpr int WINDOW_HEIGHT = 1080;

#define WHITE_COLOR {.r = 1, .g = 1, .b = 1}

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

	Vec3& position = camera.transform.position;
	float yaw = -camera.transform.rotation.yaw;

	Vec3 playerForward = Vec3{sinf(yaw), 0, cosf(yaw)} * -1;
	Vec3 playerRight = {cosf(yaw), 0, -sinf(yaw)};

	if (window.isKeyPressed(GLFW_KEY_W)) {
		position += playerForward * cameraSpeed;
	}

	if (window.isKeyPressed(GLFW_KEY_S)) {
		position -= playerForward * cameraSpeed;
	}

	if (window.isKeyPressed(GLFW_KEY_D)) {
		position += playerRight * cameraSpeed;
	}

	if (window.isKeyPressed(GLFW_KEY_A)) {
		position -= playerRight * cameraSpeed;
	}
}

auto main(int argc, char* argv[]) -> int {
	Window window("Ember", WINDOW_WIDTH, WINDOW_HEIGHT);
	Renderer renderer(window);

	Camera mainCamera;
	DirectionalLight sun;

	mainCamera.fov = 60.f;
	mainCamera.aspect = (float)WINDOW_WIDTH / WINDOW_HEIGHT;
	mainCamera.transform.position = {0, 1, 0};

	Cube cube(1, {.b = 1});

	Floor floor(WHITE_COLOR);

	while (!window.shouldClose()) {
		updateCamera(mainCamera, window);

		renderer.beginScene(mainCamera, sun);

		renderer.draw(cube, {.position = {0, 0.5, -2}});
		renderer.draw(floor, {
								 .scale = 1000,
								 .rotation = {.pitch = -M_PI / 2},
								 .position = {0, 0, -2},
							 });

		renderer.endScene();
	}
}
