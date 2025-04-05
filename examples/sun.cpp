#include "shared.hpp"

constexpr uint32_t WINDOW_WIDTH{800};
constexpr uint32_t WINDOW_HEIGHT{600};

int main(void) {
	engine::init();

	Window window({
		.width = WINDOW_WIDTH,
		.height = WINDOW_HEIGHT,
		.title = "Basic Etna",
		.captureMouse = true,
	});

	Scene scene;

	MeshNode cube = scene.createMeshNode({
		.name = "Cube",
		.mesh = engine::getCube(),
		.material = engine::createColorMaterial(BLUE),
	});

	CameraNode playerCamera = scene.createCameraNode({
		.name = "PlayerCamera",
		.transform = {.position = {0, 0, 4}},
	});

	scene.addLight({
		.name = "Sun",
		.direction = {-1, -1, 0},
	});

	// rendering
	Renderer renderer({});

	scene.print();

	while (!window.shouldClose()) {
		window.pollEvents();

		updateFirstPersonCamera(playerCamera, window);

		cube->rotate(0, 0.01, 0.01);

		renderer.beginFrame(window);

		scene.render(renderer, playerCamera, {.ambient = WHITE * 0.1});

		renderer.endFrame();

		window.swapBuffers();
	}

	return 0;
}
