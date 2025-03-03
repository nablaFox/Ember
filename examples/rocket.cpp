#include "ember/physics.hpp"
#include "shared.hpp"

constexpr int WINDOW_WIDTH = 1000;
constexpr int WINDOW_HEIGHT = 800;
constexpr float GRAVITY = 9.81;
constexpr float PIXELS_PER_METER = 5;

float kineticEnergy(float mass, float vel) {
	return mass * square(vel) / 2;
}

float decelerationForce(float height, float mass, float vel) {
	return (mass * GRAVITY) + (kineticEnergy(mass, vel) / height);
}

auto main(int argc, char* argv[]) -> int {
	Window window("Basic Ember", WINDOW_WIDTH, WINDOW_HEIGHT);
	Renderer renderer(window);

	FirstPersonCamera playerCamera({
		.fov = 70,
		.aspect = (float)WINDOW_WIDTH / WINDOW_HEIGHT,
		.far = 500,
		.cameraSpeed = 6.f,
		.transform = {.position = {0, 0.5 * PIXELS_PER_METER, 3 * PIXELS_PER_METER}},
	});

	Camera rocketCamera{
		.fov = 70,
		.aspect = (float)WINDOW_WIDTH / WINDOW_HEIGHT,
		.far = 500,
		.transform = {.pitch = M_PI / 2},
	};

	Floor floor(PURPLE.setAlpha(0.3));

	window.caputureMouse(true);

	const float mass = 1100;
	const float height = 300;
	const float initialVelocity = 20;
	const float rocketHeight = 2;
	const float halfRocketHeight = rocketHeight * PIXELS_PER_METER / 2;

	OutlinedBrick rocketShape({.width = 1, .height = rocketHeight});

	PhysicalObject rocket{
		.mass = mass,
		.vel = {0, -initialVelocity, 0},
		.transform = {.position = {0, height, 0}},
		.shape = &rocketShape,
	};

	PhysicalSystem system({
		.pixelsPerMeter = PIXELS_PER_METER,
		.timeStep = 1.f / 1000,
	});

	system.addObject(&rocket);

	std::chrono::high_resolution_clock::time_point lastTime =
		std::chrono::high_resolution_clock::now();

	system.applyGlobalForce({0, mass * -GRAVITY, 0});

	float arrived = false;

	Camera* cameraToUse = &playerCamera;

	const float dForce =
		decelerationForce(height - rocketHeight / 2.f, mass, initialVelocity);

	while (!window.shouldClose()) {
		if (!arrived) {
			rocket.force = {0, dForce, 0};
			const float rocketHeight =
				(rocket.transform.position[1] - halfRocketHeight) / PIXELS_PER_METER;

			std::cout << "Height: " << rocketHeight << "m "
					  << "Velocity: " << rocket.vel[1] << "m/s\n"
					  << std::fixed << std::setprecision(2);

			std::cout << "Contact in: "
					  << rocket.vel[1] * mass / (mass * GRAVITY - dForce) << "s\n"
					  << std::endl;
		}

		system.update(renderer.getDeltaTime());

		rocketCamera.transform.position = rocket.transform.position + Vec3{0, 0, 3};

		if (rocket.transform.position[1] <= halfRocketHeight) {
			rocket.transform.position[1] = halfRocketHeight;
		}

		if (rocket.vel[1] > -1e-4) {
			arrived = true;

			std::cout << "Real fall time: "
					  << std::chrono::duration<float>(
							 std::chrono::high_resolution_clock::now() - lastTime)
							 .count()
					  << "s\n";

			std::cout << "Simulation fall time: " << system.getSimulationTime()
					  << "s\n";

			std::cout << "Impact velocity: " << rocket.vel[1] << "m/s\n";
		}

		if (window.isKeyClicked(GLFW_KEY_TAB)) {
			cameraToUse =
				cameraToUse == &rocketCamera ? &playerCamera : &rocketCamera;
		}

		playerCamera.update(window, renderer.getDeltaTime());

		renderer.beginScene(*cameraToUse, {});

		system.draw(renderer);

		floor.draw(renderer);

		renderer.endScene();

		debug(window, renderer, *cameraToUse);
	}

	return 0;
}
