#include "ember/camera.hpp"
#include "ember/window.hpp"
#include "ember/default_materials.hpp"
#include "ember/shapes.hpp"
#include "ember/renderer.hpp"
#include "physics.hpp"

using namespace ember;

struct FirstPersonCamera : Camera {
	struct CreateInfo {
		float fov{70};
		float aspect{1.77};
		float near{0.1f};
		float far{100.f};
		float cameraSpeed{0.1f};
		float sensitivity{0.001f};
		float flyAround{false};
		WorldTransform transform;
	};

	FirstPersonCamera(CreateInfo info)
		: Camera(info.fov, info.aspect, info.near, info.far, info.transform),
		  flyAround(info.flyAround),
		  sensitivity(info.sensitivity),
		  cameraSpeed(info.cameraSpeed) {}

	float flyAround = false;
	float sensitivity = 0.001f;
	float cameraSpeed = 0.1f;

	void update(Window& window, float deltaTime) {
		// CHECK multiplying by deltaTime gives weird artifacts
		transform.yaw += window.mouseDeltaX() * sensitivity;
		transform.pitch += window.mouseDeltaY() * sensitivity;

		if (transform.pitch > M_PI / 2) {
			transform.pitch = M_PI / 2;
		} else if (transform.pitch < -M_PI / 2) {
			transform.pitch = -M_PI / 2;
		}

		Vec3& position = transform.position;
		float yaw = -transform.yaw;

		const Vec3 playerForward =
			flyAround ? forward() : Vec3{sinf(yaw), 0, cosf(yaw)} * -1;

		const Vec3 playerRight =
			flyAround ? right() : Vec3{cosf(yaw), 0, -sinf(yaw)};

		const Vec3 up = flyAround ? this->up() : Vec3{0, 0, 0};

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
			  .lineThickness = 0.00002,
		  })),
		  m_subGridMaterial(gridMaterialTemplate.create({
			  .color = color * 0.8,
			  .lines = 1000 * 2,
			  .lineThickness = 0.00002,
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

struct OutlineMaterialParams {
	Color color{BLACK};
	float thickness{0.01};
};

const inline MaterialTemplate<OutlineMaterialParams> outlineMaterialTemplate({
	.shaders = {"ember/default.vert.spv", "cube_outline.frag.spv"},
});

struct OutlinedBrick : Drawable {
	struct CreateInfo {
		float width{1};
		float height{1};
		float depth{1};
		Color color{WHITE};
		Color borderColor{BLACK};
		bool transparency{false};
	};

	OutlinedBrick(CreateInfo info)
		: m_outlineMaterial(outlineMaterialTemplate.create(
			  {
				  .color = info.borderColor,
				  .thickness = 0.01,
			  },
			  {.transparency = info.transparency})),
		  m_brick(info.width, info.height, info.depth) {
		m_brick.setColor(info.color);
	}

	void draw(Renderer& renderer, WorldTransform transform = {}) override {
		renderer.draw(m_brick, transform, &m_outlineMaterial);
	}

private:
	Material m_outlineMaterial;
	Brick m_brick;
};

inline void debug(Window& window, Renderer& renderer, Camera& camera) {
	if (window.isKeyPressed(GLFW_KEY_1)) {
		std::cout << "FPS:" << renderer.getFps() << std::endl;
	}

	if (window.isKeyPressed(GLFW_KEY_2)) {
		std::cout << "DeltaTime:" << renderer.getDeltaTime() << std::endl;
	}

	if (window.isKeyPressed(GLFW_KEY_3)) {
		std::cout << "Camera Position:" << std::endl;
		camera.transform.position.print();

		std::cout << "Camera Yaw:" << camera.transform.yaw << std::endl;
		std::cout << "Camera Pitch:" << camera.transform.pitch << std::endl;
		std::cout << "Camera Roll:" << camera.transform.roll << std::endl;
	}
}
