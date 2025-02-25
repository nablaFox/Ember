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

struct Floor {
	Floor(Color color)
		: m_mainGridMaterial(gridMaterialTemplate.create({
			  .color = color,
			  .lines = 1000,
			  .lineThickness = 0.00001,
		  })),
		  m_subGridMaterial(gridMaterialTemplate.create({
			  .color = color - Color::RGBA(0, 0, 0, 30),
			  .lines = 1000 * 2,
			  .lineThickness = 0.00001,
		  })) {}

	void draw(Renderer& renderer, float height) {
		renderer.draw(m_mainGrid,
					  {
						  .scale = 1000,
						  .pitch = -M_PI / 2,
						  .position = {0, height, 0},
					  },
					  &m_mainGridMaterial);

		renderer.draw(m_subGrid,
					  {
						  .scale = 1000,
						  .pitch = -M_PI / 2,
						  .position = {0, height, 0},
					  },
					  &m_subGridMaterial);
	}

private:
	Material m_mainGridMaterial;
	Material m_subGridMaterial;
	Square m_mainGrid;
	Square m_subGrid;
};

auto main(int argc, char* argv[]) -> int {
	Window window("Ember", WINDOW_WIDTH, WINDOW_HEIGHT);
	Renderer renderer(window);

	Camera playerCamera{
		.fov = 60.f,
		.aspect = (float)WINDOW_WIDTH / WINDOW_HEIGHT,
		.transform = {.position = {0, 1, 0}},
	};

	DirectionalLight sun;

	Cube cube;
	cube.setColor(RED_COLOR);

	Floor floor(Color::RGBA(167, 152, 220, 80));

	while (!window.shouldClose()) {
		movePlayer(playerCamera, window, renderer.getDeltatime());

		renderer.beginScene(playerCamera, sun);

		renderer.draw(cube, {.yaw = M_PI / 4, .position = {2, 0.5, -4}});

		floor.draw(renderer, 0);

		renderer.endScene();
	}

	return 0;
}
