#include "shared.hpp"
#include "forward_renderer.hpp"

constexpr uint32_t WINDOW_WIDTH = 800;
constexpr uint32_t WINDOW_HEIGHT = 600;

int main(int argc, char* argv[]) {
	Engine engine;

	Window window({
		.width = WINDOW_WIDTH,
		.height = WINDOW_HEIGHT,
		.title = "Basic Ember",
		.captureMouse = true,
	});

	FirstPersonCamera playerCamera({
		.fov = 70,
		.aspect = (float)WINDOW_WIDTH / WINDOW_HEIGHT,
		.cameraSpeed = 6.f,
		.transform = {.position = {0, 1, 0}},
	});

	MaterialHandle gridMaterial = Engine::createGridMaterial({
		.color = BLUE,
		.gridSpacing = 0.1,
		.thickness = 0.005,
	});

	Scene scene;

	SceneNode& sphere1 = scene.addMesh(
		Engine::createSphere(BLUE * 0.08),
		{.scale = 0.5, .pitch = M_PI / 2, .position = {1.5, 0.5, -5}}, gridMaterial);

	SceneNode& sphere2 =
		scene.addMesh(Engine::createSphere(GREEN), {.position = {0, 2.5, -9}},
					  Engine::getPointMaterial());

	SceneNode& outlinedBrick = scene.addMesh(
		Engine::createTexturedCube(), {.yaw = M_PI / 4, .position = {-2.5, 0.5, -5}},
		Engine::brickOutlinedMaterial({}));

	SceneNode& floor = scene.addNode(Floor({.color = PURPLE.setAlpha(0.3)}));

	Renderer renderer({});

	while (!window.shouldClose()) {
		renderer.beginFrame();

		renderer.renderScene(scene, window, playerCamera);

		renderer.endFrame();

		window.swapBuffers();

		playerCamera.update(window, 0.01);
	}
}
