#include "ember/camera.hpp"
#include "ember/window.hpp"
#include "ember/default_materials.hpp"
#include "ember/shapes.hpp"
#include "ember/renderer.hpp"

struct FirstPersonCamera {
	struct CreateInfo {
		float fov{90.f};
		float aspect{1.77f};
		Vec3 position{0, 0, 0};
	};

	FirstPersonCamera(CreateInfo info)
		: camera({
			  .fov = info.fov,
			  .aspect = info.aspect,
			  .transform = {.position = info.position},
		  }) {}

	Camera camera;

	float flyAround = false;
	float sensitivity = 0.001f;

	void update(Window& window, float deltaTime) {
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
};

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

	void draw(Renderer& renderer, float height = 0) {
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

struct OutlinedCube {
	OutlinedCube()
		: m_outlineMaterial(Material({
			  .shaders = {"ember/default.vert.spv", "cube_outline.frag.spv"},
		  })) {
		m_cube.setColor(WHITE);
	}

	void draw(Renderer& renderer, WorldTransform transform = {}) {
		renderer.draw(m_cube, transform, &m_outlineMaterial);
	}

private:
	Material m_outlineMaterial;
	Cube m_cube;
};
