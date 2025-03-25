#include "shared.hpp"
#include "engine.hpp"
#include "scene.hpp"
#include "primitives.hpp"

using namespace etna;

constexpr uint32_t WINDOW_WIDTH = 800;
constexpr uint32_t WINDOW_HEIGHT = 600;

int main(int argc, char* argv[]) {
	Engine engine;

	Window window({
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

	FirstPersonCamera playerCamera({
		.fov = 70,
		.aspect = (float)WINDOW_WIDTH / WINDOW_HEIGHT,
		.cameraSpeed = 6.f,
		.transform = {.position = {0, 0, 1}},
	});

	Scene scene;

	SceneNode& brick = scene.addMesh(utils::createTexturedBrick(1, 1, 2),
									 {.pitch = 0, .position = {0.f, 0.f, -2.f}});

	while (!window.shouldClose()) {
		engine.beginFrame();

		VkViewport viewport1{
			.x = 0,
			.y = 0,
			.width = (float)WINDOW_WIDTH / 2,
			.height = (float)WINDOW_HEIGHT,
		};

		VkViewport viewport2{
			.x = (float)WINDOW_WIDTH / 2,
			.y = 0,
			.width = (float)WINDOW_WIDTH / 2,
			.height = (float)WINDOW_HEIGHT,
		};

		engine.renderScene(scene, window, playerCamera,
						   {
							   .viewport = viewport1,
						   });

		engine.renderScene(scene, window, playerCamera,
						   {
							   .viewport = viewport2,
							   .colorLoadOp = VK_ATTACHMENT_LOAD_OP_LOAD,
						   });

		engine.renderScene(scene, window2, playerCamera);

		engine.endFrame();

		window.swapBuffers();

		window2.swapBuffers();

		playerCamera.update(window, 0.016f);
	}

	return 0;
}
