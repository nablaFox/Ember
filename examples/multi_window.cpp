#include "shared.hpp"

using namespace etna;

constexpr float WINDOW_WIDTH{800};
constexpr float WINDOW_HEIGHT{600};

int main(void) {
	engine::init();

	Window window1({
		.width = (uint32_t)WINDOW_WIDTH,
		.height = (uint32_t)WINDOW_HEIGHT,
		.title = "Etna Window 1",
		.captureMouse = true,
	});

	Window window2({
		.width = (uint32_t)WINDOW_WIDTH,
		.height = (uint32_t)WINDOW_HEIGHT,
		.title = "Etna Window 2",
	});

	Scene scene;

	SceneNode root = scene.createRoot("root", {});

	root.addMesh("Brick", engine::getCube(),
				 {
					 .position = {0.f, 0.f, -2.f},
					 .scale = {1.f, 1.f, 2.f},
				 });

	Viewport const viewport1{
		.width = WINDOW_WIDTH / 2.f,
	};

	Viewport const viewport2{
		.x = WINDOW_WIDTH / 2.f,
		.width = WINDOW_WIDTH / 2.f,
	};

	CameraNode& camera = root.addCamera("Camera1", {.position = {0, 0, 1}});

	Renderer renderer({});

	bool shouldClose{false};

	while (!shouldClose) {
		window1.pollEvents();
		window2.pollEvents();

		updateFirstPersonCamera(camera, window1);

		if (window1.shouldClose() || window2.shouldClose()) {
			shouldClose = true;
		}

		renderer.begin();

		renderer.beginDrawing(window1, camera);

		renderer.renderScene(scene, {.viewport = viewport1});
		renderer.renderScene(scene, {.viewport = viewport2});

		renderer.endDrawing();

		renderer.beginDrawing(window2, camera);

		renderer.renderScene(scene);

		renderer.endDrawing();

		renderer.execute();

		window1.swapBuffers();

		window2.swapBuffers();
	}

	return 0;
}
