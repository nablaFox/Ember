#include "ember/physics.hpp"
#include "shared.hpp"

constexpr int WINDOW_WIDTH = 1000;
constexpr int WINDOW_HEIGHT = 800;
constexpr float GRAVITY = 9.81;
constexpr float WU_PER_METER = 0.04;

float kineticEnergy(float mass, float vel) {
	return mass * square(vel) / 2;
}

float decelerationForce(float height, float mass, float vel) {
	return (mass * GRAVITY) + (kineticEnergy(mass, vel) / height);
}

struct RocketShape : Drawable {
	RocketShape(float width, float height)
		: m_rocketBody({
			  .width = width,
			  .height = height,
			  .depth = width,
		  }),
		  m_rocketWidth(width),
		  m_rocketHeight(height),
		  m_rocketNose(height / 3, width * 1.2) {
		m_rocketNose.setColor(RED);
	}

	void draw(Renderer& renderer, WorldTransform transform = {}) override {
		m_rocketBody.draw(renderer, transform);

		renderer.draw(m_rocketNose, {.position = transform.position +
												 Vec3{0, m_rocketHeight / 2, 0}});
	}

	float width() const { return m_rocketWidth; }
	float height() const { return m_rocketHeight; }

private:
	float m_rocketHeight;
	float m_rocketWidth;

	OutlinedBrick m_rocketBody;
	Pyramid m_rocketNose;
};

auto main(int argc, char* argv[]) -> int {
	Window window({
		.windowName = "Rocket Demo",
		.width = WINDOW_WIDTH,
		.height = WINDOW_HEIGHT,
		.captureMouse = true,
	});

	Renderer renderer(window);

	FirstPersonCamera playerCamera({
		.fov = 90,
		.aspect = (float)WINDOW_WIDTH / WINDOW_HEIGHT,
		.cameraSpeed = 6.f,
		.transform = {.position = {0, 2, 11}},
	});

	Camera rocketCamera{
		.fov = 70,
		.aspect = (float)WINDOW_WIDTH / WINDOW_HEIGHT,
		.transform = {.pitch = M_PI / 2},
	};

	Camera* cameraToUse = &playerCamera;

	Floor floor({.color = PURPLE.setAlpha(0.3)});

	PhysicalSystem system({
		.timeStep = 1.f / 120.f,
		.worldUnitPerMeter = WU_PER_METER,
	});

	BoxShape boxShape(system.meterToWu(340));

	const float mass = 11000;
	const float height = system.meterToWu(300);
	const float initialVelocity = 40;
	const float rocketHeight = 1.5;
	const float rocketWidth = 0.75;
	const float halfRocketHeight = rocketHeight / 2.f;

	RocketShape rocketShapeTest(rocketWidth, rocketHeight);

	PhysicalObject rocket{
		.mass = mass,
		.vel = {0, -initialVelocity, 0},
		.transform = {.position = {0, height, 0}},
		.shape = &rocketShapeTest,
	};

	system.addObject(&rocket);
	system.applyGlobalForce({0, mass * -GRAVITY, 0});

	std::chrono::high_resolution_clock::time_point lastTime =
		std::chrono::high_resolution_clock::now();

	bool hasLanded = false;
	bool simulating = false;

	const float dForce = decelerationForce(
		system.wuToMeter(height - halfRocketHeight), mass, initialVelocity);

	while (!window.shouldClose()) {
		// simulate
		if (!hasLanded) {
			rocket.force = {0, dForce, 0};

			std::cout << "Height: "
					  << (rocket.pos[1] - system.wuToMeter(halfRocketHeight)) << "m "
					  << "Velocity: " << rocket.vel[1] << "m/s\n"
					  << std::fixed << std::setprecision(2);

			std::cout << "Contact in: "
					  << mass * rocket.vel[1] / (mass * GRAVITY - dForce) << "s\n"
					  << std::endl;
		}

		if (rocket.transform.position[1] <= halfRocketHeight && !hasLanded) {
			std::cout << "Real fall time: "
					  << std::chrono::duration<float>(
							 std::chrono::high_resolution_clock::now() - lastTime)
							 .count()
					  << "s\n";

			std::cout << "Simulation fall time: " << system.getSimulationTime()
					  << "s\n";

			std::cout << "Impact velocity: " << rocket.vel[1] << "m/s\n";

			hasLanded = true;
		}

		if (rocket.transform.position[1] < halfRocketHeight) {
			rocket.transform.position[1] = halfRocketHeight;
			rocket.vel[1] = 0;
		}

		if (simulating)
			system.update(renderer.getDeltaTime());

		rocketCamera.transform.position =
			rocket.transform.position + Vec3{0, 0, 0.5};
		playerCamera.update(window, renderer.getDeltaTime());

		// input
		if (window.isKeyClicked(GLFW_KEY_TAB)) {
			cameraToUse =
				cameraToUse == &rocketCamera ? &playerCamera : &rocketCamera;
		}

		if (window.isKeyClicked(GLFW_KEY_ENTER)) {
			simulating = true;
		}

		// render
		renderer.beginScene(*cameraToUse, {});

		system.draw(renderer);

		floor.draw(renderer);

		boxShape.draw(renderer, {.position = {0, boxShape.dimension / 2, 0}});

		renderer.endScene();

		debug(window, renderer, *cameraToUse);
	}

	return 0;
}
