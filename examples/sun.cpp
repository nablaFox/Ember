#include "color.hpp"
#include "default_primitives.hpp"
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

	CameraNode playerCamera = scene.createCameraNode({
		.name = "PlayerCamera",
		.transform = {.position = {0, 1.5, 6}},
	});

	scene.addLight({
		.name = "Sun",
		.direction = {1, -1, 0},
	});

	scene.createMeshNode({
		.name = "Floor",
		.mesh = engine::getSphere(),
		.transform =
			{
				.position = {0, -200, 0},
				.scale = Vec3(200),
			},
		.material = engine::createGridMaterial({
			.color = WHITE,
			.gridColor = WHITE * 0.5,
			.gridSpacing = 0.001,
			.thickness = 0.00001,
		}),
	});

	scene.createMeshNode({
		.name = "Sphere",
		.mesh = engine::getSphere(),
		.transform = {.position = {-2, 1.5, 1}},
		.material = engine::createColorMaterial(PURPLE),
	});

	MeshNode cube = scene.createMeshNode({
		.name = "Cube",
		.mesh = engine::getCube(),
		.transform = {.position = {2, 2, 0}},
		.material = engine::createColorMaterial(BLUE),
	});

	// rendering
	Renderer renderer({});

	scene.print();

	while (!window.shouldClose()) {
		window.pollEvents();

		updateFirstPersonCamera(playerCamera, window, {.flyAround = true});

		cube->rotate(0, 0.01, 0.01);

		renderer.beginFrame(window, {.clearColor = CELESTE});

		scene.render(renderer, playerCamera, {.ambient = WHITE * 0.1});

		renderer.endFrame();

		window.swapBuffers();
	}

	return 0;
}
