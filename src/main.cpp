#include <cmath>
#include "window.hpp"
#include "renderer.hpp"
#include "mesh.hpp"

constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 600;

struct Cube : Mesh {
	static constexpr int verticesCount = 8;
	static constexpr int indicesCount = 36;

	Cube(float sideLength, Color color) : Mesh(verticesCount, indicesCount) {
		for (uint32_t i = 0; i < 2; i++) {
			float zShift = 0.5f - i;

			m_vertices[0 + i * 4].position = {0.5f, -0.5f, zShift};
			m_vertices[1 + i * 4].position = {-0.5f, -0.5f, zShift};
			m_vertices[2 + i * 4].position = {-0.5f, 0.5f, zShift};
			m_vertices[3 + i * 4].position = {0.5f, 0.5f, zShift};
		}

		for (uint32_t i = 0; i < 8; i++) {
			m_vertices[i].color = color;
			m_vertices[i].position *= sideLength;
		}

		m_indices = {
			0, 1, 2, 2, 3, 0,  // Front face
			4, 5, 6, 6, 7, 4,  // Back face
			1, 5, 4, 4, 0, 1,  // Top face
			3, 7, 6, 6, 2, 3,  // Bottom face
			1, 2, 6, 6, 5, 1,  // Left face
			0, 4, 7, 7, 3, 0,  // Right face
		};
	}
};

struct Square : Mesh {
	Square(Color color) : Mesh(4, 6) {
		m_vertices[0].position = {0.5, -0.5, 0.f};
		m_vertices[1].position = {-0.5, -0.5, 0.f};
		m_vertices[2].position = {-0.5, 0.5, 0.f};
		m_vertices[3].position = {0.5, 0.5, 0.f};

		for (auto& vertex : m_vertices) {
			vertex.color = color;
		}

		m_indices = {0, 1, 2, 2, 3, 0};
	}
};

auto updateCamera(Camera& camera, const Window& window) -> void {
	static float previousMouseX = window.getMouseX();
	static float previousMouseY = window.getMouseY();

	float deltaX = window.getMouseX() - previousMouseX;
	float deltaY = window.getMouseY() - previousMouseY;

	previousMouseX = window.getMouseX();
	previousMouseY = window.getMouseY();

	constexpr float sensitivity = 0.005f;

	camera.rotate({
		.yaw = -deltaX * sensitivity,
		.pitch = -deltaY * sensitivity,
	});

	constexpr float cameraSpeed = 0.1f;

	Vec3 translation{};

	if (window.isKeyPressed(GLFW_KEY_W)) {
		translation[0] -= cameraSpeed;
	}

	if (window.isKeyPressed(GLFW_KEY_S)) {
		translation[0] += cameraSpeed;
	}

	if (window.isKeyPressed(GLFW_KEY_D)) {
		translation[1] += cameraSpeed;
	}

	if (window.isKeyPressed(GLFW_KEY_A)) {
		translation[1] -= cameraSpeed;
	}

	if (window.isKeyPressed(GLFW_KEY_SPACE)) {
		translation[2] += cameraSpeed;
	}

	if (window.isKeyPressed(GLFW_KEY_LEFT_SHIFT)) {
		translation[2] -= cameraSpeed;
	}

	camera.translate(translation);
}

int main(int argc, char* argv[]) {
	Window window("Ember", WINDOW_WIDTH, WINDOW_HEIGHT);
	Renderer::init(window);

	Renderer& renderer = Renderer::getInstance();

	Camera mainCamera;
	DirectionalLight sun;

	mainCamera.fov = 60.f;
	mainCamera.aspect = (float)WINDOW_WIDTH / WINDOW_HEIGHT;

	Square square({.r = 1});
	square.upload();

	Cube cube(1, {.b = 1});
	cube.upload();

	while (!window.shouldClose()) {
		updateCamera(mainCamera, window);

		renderer.beginScene(mainCamera, sun);

		renderer.draw(cube, {.translation = {1.5f, -0.5f, -3.f}});
		renderer.draw(square, {.translation = {-2.5f, 0.5f, -2.f}});

		renderer.endScene();
	}
}
