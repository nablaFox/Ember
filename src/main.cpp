#include <cmath>
#include "window.hpp"
#include "renderer.hpp"

constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 600;
constexpr int GRAVITY = 10;

#define SKYBLUE Color{102, 191, 255, 255}

struct Cube {
	vec3 position;
	vec3 velocity;
	float sideLength = 1;

	Cube(float sideLength, vec3 initialVel, vec3 initialPos, Color color) {
		constexpr int verticesCount = 8;
		constexpr int indicesCount = 36;

		Vertex* vertices = new Vertex[verticesCount];

		for (uint32_t i = 0; i < 2; i++) {
			float zShift = (float)(i - 0.5);

			vertices[0 + i * 4].position = {0.5, 0.5, zShift};
			vertices[1 + i * 4].position = {-0.5, 0.5, zShift};
			vertices[2 + i * 4].position = {-0.5, -0.5, zShift};
			vertices[3 + i * 4].position = {0.5, -0.5, zShift};
		}

		for (uint32_t i = 0; i < 8; i++) {
			vertices[i].color = color;
			vertices[i].position *= sideLength;
		}

		Index* indices = new Index[36]{
			0, 1, 2, 2, 3, 0,  // back face
			4, 5, 6, 6, 7, 4,  // front face
			0, 1, 5, 5, 4, 0,  // bottom face
			2, 3, 7, 7, 6, 2,  // top face
			0, 3, 7, 7, 4, 0,  // left face
			1, 2, 6, 6, 5, 1   // right face
		};

		mesh = new Mesh(std::span(vertices, verticesCount),
						std::span(indices, indicesCount));
	}

	void updateSideLength(float);

	// TEMP
	WorldTransform getTransform() { return WorldTransform{}; };
	Mesh* mesh;
};

auto updateCamera(Camera& camera, const Window& window) -> void {
	static float previousMouseX = window.getMouseX();
	static float previousMouseY = window.getMouseY();

	float deltaX = window.getMouseX() - previousMouseX;
	float deltaY = window.getMouseY() - previousMouseY;

	previousMouseX = window.getMouseX();
	previousMouseY = window.getMouseY();

	constexpr float sensitivityX = 0.1f;
	constexpr float sensitivityY = 0.1f;

	camera.rot.yaw += deltaX * sensitivityX;
	camera.rot.pitch += deltaY * sensitivityY;

	if (camera.rot.pitch > 89.f)
		camera.rot.pitch = 89.f;
	else if (camera.rot.pitch < -89.f)
		camera.rot.pitch = -89.f;

	constexpr float cameraSpeed = 1.f;

	if (window.isKeyPressed(GLFW_KEY_A)) {
		camera.position.x -= cameraSpeed;
	} else if (window.isKeyPressed(GLFW_KEY_D)) {
		camera.position.x += cameraSpeed;
	} else if (window.isKeyPressed(GLFW_KEY_W)) {
		camera.position.z -= cameraSpeed;
	} else if (window.isKeyPressed(GLFW_KEY_S)) {
		camera.position.z += cameraSpeed;
	} else if (window.isKeyPressed(GLFW_KEY_SPACE)) {
		camera.position.y += cameraSpeed;
	} else if (window.isKeyPressed(GLFW_KEY_LEFT_SHIFT)) {
		camera.position.y -= cameraSpeed;
	}
}

int main(int argc, char* argv[]) {
	Window window("Ember", WINDOW_WIDTH, WINDOW_HEIGHT);
	Renderer renderer(window);

	Cube cube(1, {0, 0, 15}, {0}, SKYBLUE);

	SceneData defaultScene;

	while (!window.shouldClose()) {
		updateCamera(defaultScene.camera, window);

		float dt = renderer.getFrameTime();
		cube.velocity += vec3{0, GRAVITY, 0} * dt;
		cube.position += cube.velocity * dt;

		renderer.beginScene(defaultScene);

		renderer.draw(*cube.mesh, cube.getTransform());

		renderer.endScene();
	}
}
