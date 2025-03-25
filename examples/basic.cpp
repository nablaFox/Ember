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
	});

	FirstPersonCamera playerCamera({
		.fov = 70,
		.aspect = (float)WINDOW_WIDTH / WINDOW_HEIGHT,
		.cameraSpeed = 6.f,
		.transform = {.position = {0, 1, 0}},
	});

	Scene scene;

	SceneNode sphere = scene.addMesh(utils::createSphere(), {});

	// SceneConfigs sceneConfigs

	while (!window.shouldClose()) {
		engine.beginFrame();

		engine.renderScene(scene, window, playerCamera);

		engine.endFrame();

		window.swapBuffers();
	}

	return 0;
}
