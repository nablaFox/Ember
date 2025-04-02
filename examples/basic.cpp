#include "shared.hpp"

constexpr uint32_t WINDOW_WIDTH = 800;
constexpr uint32_t WINDOW_HEIGHT = 600;

int main(void) {
	engine::init();

	Window window({
		.width = WINDOW_WIDTH,
		.height = WINDOW_HEIGHT,
		.title = "Basic Etna",
		.captureMouse = true,
	});

	Scene scene;

	SceneNode root = scene.createRoot("root", {});

	// geometry
	root.addMesh("Sphere1", engine::createSphere(BLUE * 0.08),
				 {
					 .scale = 0.5,
					 .pitch = M_PI / 2,
					 .position = {1.5, 0.5, -5},
				 },
				 engine::createGridMaterial({
					 .gridColor = BLUE,
					 .gridSpacing = 0.1,
					 .thickness = 0.005,
				 }));

	root.addMesh("Sphere2", engine::createSphere(GREEN), {.position = {0, 2.5, -9}},
				 engine::getPointMaterial());

	root.addModel("OutlinedBrick", createOutlinedBrick({}),
				  {
					  .yaw = M_PI / 4,
					  .position = {-2.5, 0.5, -5},
				  });

	root.addModel("Floor", createFloor({.color = PURPLE.setAlpha(0.3)}));

	// camera
	CameraNode& playerCamera =
		root.addCamera("PlayerCamera", {.fov = 70, .aspect = window.getAspect()},
					   {.position = {0, 1, 0}});

	// rendering
	Renderer renderer({});

	while (!window.shouldClose()) {
		window.pollEvents();

		updateFirstPersonCamera(playerCamera, window);

		renderer.beginFrame();

		renderer.renderScene(scene, window);

		renderer.endFrame();

		window.swapBuffers();
	}
}
