#include "shared.hpp"
#include "forward_renderer.hpp"

using namespace etna;

constexpr uint32_t WINDOW_WIDTH = 800;
constexpr uint32_t WINDOW_HEIGHT = 600;

int main(int argc, char* argv[]) {
	Engine engine;

	Window window1({
		.width = WINDOW_WIDTH,
		.height = WINDOW_HEIGHT,
		.title = "Etna Window",
		.captureMouse = true,
	});

	Window window2({
		.width = WINDOW_WIDTH,
		.height = WINDOW_HEIGHT,
		.title = "Etna Window 2",
	});

	Scene scene;

	SceneNode root = scene.createRoot("root", {});

	SceneNode& brick = root.addMesh("Brick", Engine::createTexturedBrick(1, 1, 2),
									{.pitch = 0, .position = {0.f, 0.f, -2.f}});

	Viewport const viewport1{
		.x = 0,
		.y = 0,
		.width = (float)WINDOW_WIDTH / 2,
		.height = (float)WINDOW_HEIGHT,
	};

	Viewport const viewport2{
		.x = (float)WINDOW_WIDTH / 2,
		.y = 0,
		.width = (float)WINDOW_WIDTH / 2,
		.height = (float)WINDOW_HEIGHT,
	};

	Camera const sharedCamera{
		.fov = 70,
		.aspect = (float)WINDOW_WIDTH / WINDOW_HEIGHT,
	};

	CameraNode& camera1 =
		root.addCamera("Camera1", sharedCamera, {.position = {0, 0, 1}}, viewport1);

	CameraNode& camera2 =
		root.addCamera("Camera2", sharedCamera, {.position = {0, 0, 1}}, viewport2);

	Renderer renderer({});

	while (!window1.shouldClose()) {
		renderer.beginFrame();

		renderer.renderScene(scene, window1);

		renderer.renderScene(scene, window2);

		renderer.endFrame();

		window1.swapBuffers();

		window2.swapBuffers();

		updateFirstPersonCamera(camera1, window1);
		updateFirstPersonCamera(camera2, window1);
	}

	return 0;
}
