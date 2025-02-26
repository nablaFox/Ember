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

	constexpr double sensitivity = 0.001f;

	WorldTransform& transform = camera.transform;

	// CHECK multiplying by deltaTime gives weird artifacts
	transform.yaw += window.mouseDeltaX() * sensitivity;
	transform.pitch += window.mouseDeltaY() * sensitivity;

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
			  .color = color * 0.8,
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
		.aspect = (float)WINDOW_WIDTH / WINDOW_HEIGHT,
		.transform = {.position = {0, 1, 0}},
	};

	DirectionalLight sun;

	Cube cube;
	cube.setColor(BLUE);

	Floor floor(PURPLE.setAlpha(0.3));

	Circle circle;
	circle.setColor(RED);

	Sphere sphere;
	sphere.setColor(PURPLE);

	while (!window.shouldClose()) {
		movePlayer(playerCamera, window, renderer.getDeltatime());

		renderer.beginScene(playerCamera, sun);

		renderer.draw(cube, {.yaw = M_PI / 4, .position = {2, 0.5, -4}});

		floor.draw(renderer, 0);

		renderer.draw(circle, {.position = {0, 0, -3}});
		renderer.draw(sphere, {.position = {2, 0, -8}}, &pointMaterial);

		renderer.endScene();
	}

	return 0;
}
