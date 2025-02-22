#pragma once

#include <cassert>
#include <cmath>
#include "ignis/ignis.hpp"
#include "math.hpp"

using namespace ignis;

class Window;

struct Rot {
	float yaw{0}, pitch{0}, roll{0};

	Mat4 getRotMatrix() const {
		Mat4 yawMat = {
			{cosf(yaw), 0, sinf(yaw), 0},
			{0, 1, 0, 0},
			{-sinf(yaw), 0, cosf(yaw), 0},
			{0, 0, 0, 1},
		};

		Mat4 pitchMat = {
			{1, 0, 0, 0},
			{0, cosf(pitch), -sinf(pitch), 0},
			{0, sinf(pitch), cosf(pitch), 0},
			{0, 0, 0, 1},
		};

		return pitchMat * yawMat;
	}
};

struct WorldTransform {
	float scale = 1;
	Rot rotation = {0, 0, 0};
	Vec3 translation = {0, 0, 0};

	Mat4 getWorldMatrix() const {
		return getTransMatrix() * getScaleMatrix() * getRotMatrix();
	}

	Mat4 getTransMatrix() const {
		return {
			{1, 0, 0, translation[0]},
			{0, 1, 0, translation[1]},
			{0, 0, 1, translation[2]},
			{0, 0, 0, 1},
		};
	}

	Mat4 getScaleMatrix() const {
		return {
			{scale, 0, 0, 0},
			{0, scale, 0, 0},
			{0, 0, scale, 0},
			{0, 0, 0, 1},
		};
	}

	Mat4 getRotMatrix() const { return rotation.getRotMatrix(); }
};

struct Camera {
	float fov = 90.f;
	float aspect = 1.77f;

	WorldTransform transform;

	Mat4 V = {
		{1, 0, 0, 0},  // right
		{0, 1, 0, 0},  // up
		{0, 0, 1, 0},  // forward
		{0, 0, 0, 1},
	};

	void rotate(Rot rotation) {
		if (rotation.pitch > 89.f)
			rotation.pitch = 89.f;
		else if (rotation.pitch < -89.f)
			rotation.pitch = -89.f;

		// we need the inverse
		// and for a rotation matrix, the inverse is the transpose
		V = V * rotation.getRotMatrix().transpose();
	}

	void translate(Vec3 translation) {
		Vec3 forward = {
			V(2, 0),
			V(2, 1),
			V(2, 2),
		};

		Vec3 right = {
			V(0, 0),
			V(0, 1),
			V(0, 2),
		};

		Vec3 up = {
			V(1, 0),
			V(1, 1),
			V(1, 2),
		};

		std::cout << "forward" << std::endl;
		forward.print();

		transform.translation += (forward * translation[0] + right * translation[1] +
								  up * translation[2]) *
								 -1;
	}

	Mat4 getViewMatrix() {
		Mat4 position = transform.getTransMatrix();

		return V * position;
	}

	Mat4 getProjMatrix() {
		const float near = 1.f;
		const float far = 1e2f;

		const float fovAngle = fov * M_PI / 180;

		const float top = near * tanf(fovAngle / 2);

		const float right = top * aspect;

		return {
			{near / right, 0, 0, 0},
			{0, -near / top, 0, 0},
			{0, 0, far / (near - far), near * far / (near - far)},
			{0, 0, -1, 0},
		};
	}

	Mat4 getViewProjMatrix() { return getProjMatrix() * getViewMatrix(); }
};

struct Color {
	float r, g, b, a;
};

struct DirectionalLight {
	Vec3 direction;
	Color color;
};

struct SceneData {
	Mat4 viewproj;
	Color ambientColor;
	DirectionalLight sun;
};

struct PushConstants {
	Mat4 worldTransform;
	VkDeviceAddress verticesAddress;
};

class Mesh;

class Renderer {
public:
	static Renderer* g_instance;

	static void init(const Window& window) { g_instance = new Renderer(window); }

	static Renderer& getInstance() {
		if (g_instance == nullptr) {
			assert("Renderer not initialized");
		}

		return *g_instance;
	}

public:
	void beginScene(Camera camera,
					DirectionalLight sun,
					// TEMP
					Color ambientColor = Color{102, 191, 255, 255});

	void endScene();

	void draw(Mesh&, WorldTransform);

	Device& getDevice() { return *m_device; }

	~Renderer();

private:
	Renderer(const Window&);

	Device* m_device;
	ColorImage* m_drawImage;
	DepthImage* m_depthImage;
	// TODO double buffering
	Command* m_cmd;
	Buffer* m_sceneDataUBO;
	Pipeline* m_pipeline;

	Swapchain* m_swapchain;
	Semaphore* m_finishedRendering;
	Fence* m_waitForRenderingCompletion;

	uint32_t graphicsQueue;
	uint32_t uploadQueue;

	DrawAttachment m_drawAttachment;
	DepthAttachment m_depthAttachment;

	PushConstants m_pushConstants;
};
