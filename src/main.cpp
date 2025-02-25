#include <cmath>
#include "window.hpp"
#include "renderer.hpp"
#include "shapes.hpp"
#include "device.hpp"
#include "default_materials.hpp"

constexpr int WINDOW_WIDTH = 1920;
constexpr int WINDOW_HEIGHT = 1080;

void movePlayer(Camera& camera, const Window& window, float deltaTime) {
	static float flyAround = false;

	constexpr float sensitivity = 5.f;

	WorldTransform& transform = camera.transform;

	static float targetYaw = transform.yaw;
	static float targetPitch = transform.pitch;

	targetYaw += window.mouseDeltaX() * sensitivity * deltaTime;
	targetPitch += window.mouseDeltaY() * sensitivity * deltaTime;

	camera.transform.yaw = lerp(transform.yaw, targetYaw, 0.5);
	camera.transform.pitch = lerp(transform.pitch, targetPitch, 0.5);

	if (transform.pitch > M_PI / 2) {
		transform.pitch = M_PI / 2;
	} else if (transform.pitch < -M_PI / 2) {
		transform.pitch = -M_PI / 2;
	}

	constexpr float cameraSpeed = 0.1f;

	Vec3& position = transform.position;
	float yaw = -transform.yaw;

	const Vec3 playerForward =
		flyAround ? camera.forward() : Vec3{sinf(yaw), 0, cosf(yaw)} * -1;

	const Vec3 playerRight =
		flyAround ? camera.right() : Vec3{cosf(yaw), 0, -sinf(yaw)};

	const Vec3 up = flyAround ? camera.up() : Vec3{0, 0, 0};

	if (window.isKeyPressed(GLFW_KEY_0)) {
		position = {0, 1, 0};
	}

	if (window.isKeyPressed(GLFW_KEY_F)) {
		flyAround = true;
	}

	if (window.isKeyPressed(GLFW_KEY_G)) {
		flyAround = false;
	}

	if (window.isKeyPressed(GLFW_KEY_W)) {
		position += playerForward * cameraSpeed;
	}

	if (window.isKeyPressed(GLFW_KEY_S)) {
		position -= playerForward * cameraSpeed;
	}

	if (window.isKeyPressed(GLFW_KEY_D)) {
		position += playerRight * cameraSpeed;
	}

	if (window.isKeyPressed(GLFW_KEY_A)) {
		position -= playerRight * cameraSpeed;
	}

	if (window.isKeyPressed(GLFW_KEY_SPACE)) {
		position += up * cameraSpeed;
	}

	if (window.isKeyPressed(GLFW_KEY_LEFT_SHIFT)) {
		position -= up * cameraSpeed;
	}
}

auto main(int argc, char* argv[]) -> int {
	Window window("Ember", WINDOW_WIDTH, WINDOW_HEIGHT);
	Renderer renderer(window);

	Camera playerCamera{
		.fov = 60.f,
		.aspect = (float)WINDOW_WIDTH / WINDOW_HEIGHT,
		.transform = {.position = {0, 1, 0}},
	};

	DirectionalLight sun;

	Material gridMaterial1 = gridMaterialTemplate.create({
		.color = Color::RGBA(167, 152, 220, 80),
		.lines = 1000,
		.lineThickness = 0.00001,
	});

	Material gridMaterial2 = gridMaterialTemplate.create({
		.color = Color::RGBA(167, 152, 220, 30),
		.lines = 1000 * 2,
		.lineThickness = 0.00001,
	});

	Square floor1(&gridMaterial1);
	Square floor2(&gridMaterial2);

	Cube cube;
	cube.setColor(RED_COLOR);

	while (!window.shouldClose()) {
		movePlayer(playerCamera, window, renderer.getDeltatime());

		renderer.beginScene(playerCamera, sun);

		renderer.draw(cube, {
								.yaw = M_PI / 4,
								.position = {2, 0.5, -4},
							});

		renderer.draw(floor1, {
								  .scale = 1000,
								  .pitch = -M_PI / 2,
								  .position = {0, 0, -2},
							  });

		renderer.draw(floor2, {
								  .scale = 1000,
								  .pitch = -M_PI / 2,
								  .position = {0, 0, -2},
							  });

		renderer.endScene();
	}

	return 0;
}
