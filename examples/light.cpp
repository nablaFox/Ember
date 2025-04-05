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
		.name = "Light",
		.direction = {1, -1, 0},
		.intensity = 0.8,
		.color = WHITE,
	});

	// rendering
	Renderer renderer({});

	while (!window.shouldClose()) {
		window.pollEvents();

		updateFirstPersonCamera(playerCamera, window);

		renderer.beginFrame(window);

		scene.render(renderer, playerCamera);

		renderer.endFrame();

		window.swapBuffers();
	}

	return 0;
}
