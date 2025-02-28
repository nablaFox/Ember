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

	Material gridMaterial = gridMaterialTemplate.create(
		{
			.color = BLUE,
			.lines = 20,
			.lineThickness = 0.005,
		},
		{.transparency = false});

	Sphere sphere1;
	sphere1.setColor(BLUE * 0.08);

	Sphere sphere2;
	sphere2.setColor(GREEN);

	OutlinedBrick brick;

	Floor floor(PURPLE.setAlpha(0.3));

	window.caputureMouse(true);

	while (!window.shouldClose()) {
		renderer.beginScene(playerCamera.camera, {});

		playerCamera.update(window, renderer.getDeltaTime());

		brick.draw(renderer, {.yaw = M_PI / 4, .position = {-2.5, 0.5, -5}});

		renderer.draw(sphere1,
					  {.scale = 0.5, .pitch = M_PI / 2, .position = {1.5, 0.5, -5}},
					  &gridMaterial);

		renderer.draw(sphere2, {.position = {0, 2.5, -9}}, &pointMaterial);

		floor.draw(renderer);

		renderer.endScene();
	}

	return 0;
}
