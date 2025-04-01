#include "shared.hpp"
#include "forward_renderer.hpp"

using namespace etna;

constexpr uint32_t WINDOW_WIDTH = 800;
constexpr uint32_t WINDOW_HEIGHT = 600;

int main(int argc, char* argv[]) {
	Engine engine;

	Window window1({
		.width = 800,
		.height = 600,
		.title = "Etna Window",
		.captureMouse = true,
	});

	Window window2({
		.width = 800,
		.height = 600,
		.title = "Etna Window 2",
	});

	FirstPersonCamera camera1({
		.fov = 70,
		.aspect = (float)WINDOW_WIDTH / WINDOW_HEIGHT,
		.cameraSpeed = 6.f,
		.transform = {.position = {0, 0, 1}},
	});
	camera1.viewport = {
		.x = 0,
		.y = 0,
		.width = (float)WINDOW_WIDTH / 2,
		.height = (float)WINDOW_HEIGHT,
	};

	FirstPersonCamera camera2 = camera1;
	camera2.viewport = {
		.x = (float)WINDOW_WIDTH / 2,
		.y = 0,
		.width = (float)WINDOW_WIDTH / 2,
		.height = (float)WINDOW_HEIGHT,
	};

	Scene scene;

	SceneNode root = scene.createRoot("root", {});

	SceneNode& brick = root.addMesh("Brick", Engine::createTexturedBrick(1, 1, 2),
									{.pitch = 0, .position = {0.f, 0.f, -2.f}});

	root.addCamera("Camera1", camera1);
	root.addCamera("Camera2", camera2);

	Renderer renderer({});

	while (!window1.shouldClose()) {
		renderer.beginFrame();

		renderer.renderScene(scene, window1);

		renderer.renderScene(scene, window1);

		renderer.endFrame();

		window1.swapBuffers();

		window2.swapBuffers();

		camera1.update(window1, 0.016f);
		camera2.update(window2, 0.016f);
	}

	return 0;
}
