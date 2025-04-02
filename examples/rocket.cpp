#include "engine.hpp"
#include "physics.hpp"
#include "shared.hpp"

constexpr int WINDOW_WIDTH = 1280;
constexpr int WINDOW_HEIGHT = 720;
constexpr float GRAVITY = 9.81;
constexpr float WU_PER_METER = 0.04;  // every meter is 0.04 world units (pixels)

// Units in SI (meters, kilograms)
constexpr float ROCKET_MASS = 11000;
constexpr float ROCKET_INITIAL_VELOCITY = 20;
constexpr float ROCKET_HEIGHT = 40;
constexpr float ROCKET_WIDTH = 18;
constexpr float ROCKET_INIT_HEIGHT = 300;
const Vec3 ROCKET_INITIAL_POS{0, ROCKET_INIT_HEIGHT, 0};

void simulateRocket(PhysicalObject& rocket, MeshNode& rocketMesh) {
	const float currHeight = rocket.pos[1] - (ROCKET_HEIGHT / 2);

	if (currHeight <= 1e-3) {
		return;
	}

	const float vel = rocket.vel[1];
	const float kineticEnergy = ROCKET_MASS * square(vel) / 2;

	const float weight = ROCKET_MASS * GRAVITY;
	const float decForce = weight + kineticEnergy / currHeight;

	const float contactTime = ROCKET_MASS * vel / (ROCKET_MASS * GRAVITY - decForce);

	std::cout << "Height: " << currHeight << std::endl;
	std::cout << "Velocity: " << vel << "m/s\n"
			  << std::fixed << std::setprecision(2);
	std::cout << "Contact in: " << contactTime << "s\n" << std::endl;

	rocket.force = {0, decForce - weight, 0};

	// TODO: should be updatePosition
	rocketMesh.updateTransform({.position = rocket.pos * WU_PER_METER});
}

Model createRocketModel() {
	float width = ROCKET_WIDTH * WU_PER_METER;
	float height = ROCKET_HEIGHT * WU_PER_METER;

	Model rocket = createOutlinedBrick({
		.color = WHITE,
		.outlineColor = BLACK,
		.outlineThickness = 0.01,
		.width = width,
		.height = height,
		.depth = width,
	});

	rocket.getRoot().transform.position = {0, ROCKET_INIT_HEIGHT * WU_PER_METER, 0};

	rocket.getRoot().addSubMesh("Nose", engine::getPyramid(),
								engine::createColorMaterial(RED),
								{
									.position = {0, height / 2, 0},
									// .scale = {height / 3, width * 1.2f, 1},
								});

	return rocket;
}

int main(void) {
	engine::init();

	Window window({
		.width = WINDOW_WIDTH,
		.height = WINDOW_HEIGHT,
		.title = "Etna Rocket",
		.captureMouse = true,
	});

	Camera camera({
		.fov = 70,
		.aspect = window.getAspect(),
	});

	Scene scene;
	SceneNode root = scene.createRoot("root", {});

	root.addModel("Floor", createFloor({.color = PURPLE.setAlpha(0.3)}));

	CameraNode& rocketCamera = root.addCamera(
		"RocketCamera", camera, {},
		{.x = (float)WINDOW_WIDTH * 0.5, .width = (float)WINDOW_WIDTH * 0.5});

	CameraNode& playerCamera =
		root.addCamera("PlayerCamera", camera, {.position = {0, 1, 0}},
					   {.width = (float)WINDOW_WIDTH * 0.5});

	PhysicalObject rocket{
		.mass = ROCKET_MASS,
		.vel = {0, -ROCKET_INITIAL_VELOCITY, 0},
		.pos = ROCKET_INITIAL_POS,
	};

	MeshNode& rocketMesh = root.addModel("Rocket", createRocketModel());

	PhysicalSystem system({});

	system.addObject(&rocket);

	Renderer renderer({});

	while (!window.shouldClose()) {
		window.pollEvents();

		system.update(1 / 60.f);

		// simulateRocket(rocket, rocketMesh);

		updateFirstPersonCamera(playerCamera, window);

		rocketCamera.updateTransform({
			.position = rocketMesh.getTransform().position + Vec3{0, 0, 0.5},
			.pitch = M_PI / 2,
		});

		renderer.beginFrame();

		renderer.renderScene(scene, window);

		renderer.endFrame();

		window.swapBuffers();
	}

	return 0;
}
