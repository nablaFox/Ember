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

	WorldTransform transform{};

	void rotate(Rot rotation) {
		transform.rotation.yaw += rotation.yaw;
		transform.rotation.pitch += rotation.pitch;

		if (transform.rotation.pitch > M_PI / 2) {
			transform.rotation.pitch = M_PI / 2;
		} else if (transform.rotation.pitch < -M_PI / 2) {
			transform.rotation.pitch = -M_PI / 2;
		}
	}

	void translate(Vec3 translation) {
		Rot rot = transform.rotation;

		Vec3 forward = {
			sinf(-rot.yaw) * cosf(rot.pitch),
			sinf(rot.pitch),
			cosf(-rot.yaw) * cosf(rot.pitch),
		};

		Vec3 right = forward.cross({0, 1, 0}).normalize();
		right *= -1;

		Vec3 up = right.cross(forward).normalize();
		up *= -1;

		transform.translation +=
			forward * translation[0] + right * translation[1] + up * translation[2];
	}

	Mat4 getViewMatrix() {
		Mat4 viewTranslation = {
			{1, 0, 0, -transform.translation[0]},
			{0, 1, 0, -transform.translation[1]},
			{0, 0, 1, -transform.translation[2]},
			{0, 0, 0, 1},
		};

		Mat4 viewRotation = transform.getRotMatrix().transpose();

		return viewRotation.transpose() * viewTranslation;
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
