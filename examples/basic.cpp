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

	SceneNode root = scene.createRoot("root", {});

	// geometry
	root.addMesh("Sphere1", engine::getSphere(),
				 {
					 .position = {1.5, 0.5, -5},
					 .pitch = M_PI / 2,
					 .scale = Vec3(0.5),
				 },
				 engine::createGridMaterial({
					 .color = BLUE * 0.08,
					 .gridColor = BLUE,
					 .gridSpacing = 0.1,
					 .thickness = 0.005,
				 }));

	root.addMesh("Sphere2", engine::getSphere(), {.position = {0, 2.5, -9}},
				 engine::createPointMaterial(GREEN));

	root.addModel("OutlinedBrick", createOutlinedBrick({}),
				  {
					  .position = {-2.5, 0.5, -5},
					  .yaw = M_PI / 4,
				  });

	root.addModel("Floor", createFloor({.color = PURPLE.setAlpha(0.3)}));

	// camera
	CameraNode& playerCamera =
		root.addCamera("PlayerCamera", {.position = {0, 1, 0}});

	// rendering
	Renderer renderer({});

	while (!window.shouldClose()) {
		window.pollEvents();

		updateFirstPersonCamera(playerCamera, window);

		renderer.beginFrame(window);

		scene.render(renderer, playerCamera);

		renderer.endFrame();

		window.swapBuffers();
	}
}
