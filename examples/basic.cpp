#include <cmath>
#include "shared.hpp"

constexpr int WINDOW_WIDTH = 1920;
constexpr int WINDOW_HEIGHT = 1080;

auto main(int argc, char* argv[]) -> int {
	Window window("Basic Ember", WINDOW_WIDTH, WINDOW_HEIGHT);
	Renderer renderer(window);

	FirstPersonCamera playerCamera({
		.fov = 70,
		.aspect = (float)WINDOW_WIDTH / WINDOW_HEIGHT,
		.position = {0, 1, 0},
	});

	OutlinedCube cube;

	Sphere sphere1;
	sphere1.setColor(BLUE * 0.08);

	Sphere sphere2;
	sphere2.setColor(GREEN);

	Floor floor(PURPLE.setAlpha(0.3));

	Material gridMaterial = gridMaterialTemplate.create(
		{
			.color = BLUE,
			.lines = 20,
			.lineThickness = 0.005,
		},
		{.transparency = false});

	window.caputureMouse(true);

	while (!window.shouldClose()) {
		renderer.beginScene(playerCamera.camera, {});

		playerCamera.update(window, renderer.getDeltatime());

		cube.draw(renderer, {.yaw = M_PI / 4, .position = {-2.5, 0.5, -5}});

		renderer.draw(sphere1,
					  {.scale = 0.5, .pitch = M_PI / 2, .position = {1, 0.5, -3}},
					  &gridMaterial);

		renderer.draw(sphere2, {.position = {2, 3, -8}}, &pointMaterial);

		floor.draw(renderer);

		renderer.endScene();
	}

	return 0;
}
