#include <chrono>
#include <cmath>
#include "window.hpp"
#include "renderer.hpp"
#include "shapes.hpp"

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

int main(int argc, char* argv[]) {
	Window window("Ember", WINDOW_WIDTH, WINDOW_HEIGHT);
	Renderer::init(window);

	Renderer& renderer = Renderer::getInstance();

	Camera mainCamera;
	DirectionalLight sun;

	mainCamera.fov = 60.f;
	mainCamera.aspect = (float)WINDOW_WIDTH / WINDOW_HEIGHT;

	Cube cube(1, {.b = 1});
	cube.upload();

	while (!window.shouldClose()) {
		auto currentTime = std::chrono::high_resolution_clock::now();

		updateCamera(mainCamera, window);

		renderer.beginScene(mainCamera, sun);

		WorldTransform cubeTransform{
			.rotation = {.yaw = M_PI / 4},
			.translation = {1.5f, -0.5f, -3.f},
		};

		renderer.draw(cube, cubeTransform);

		renderer.endScene();

		std::cout << "FPS: " << renderer.getFps() << std::endl;
	}
}
