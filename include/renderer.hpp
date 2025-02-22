#pragma once

#include <cassert>
#include "types.hpp"
#include "mesh.hpp"
#include "window.hpp"
#include "camera.hpp"
#include "ignis/ignis.hpp"

using namespace ignis;

struct SceneData {
	Mat4 viewproj;
	Color ambientColor;
	DirectionalLight sun;
};

struct PushConstants {
	Mat4 worldTransform;
	VkDeviceAddress verticesAddress;
};

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
					Color ambientColor = Color{});

	void endScene();

	void draw(Mesh&, WorldTransform);

	Device& getDevice() { return *m_device; }

	~Renderer();

	static constexpr uint32_t FRAMES_IN_FLIGHT = 2;

private:
	Renderer(const Window&);

	Device* m_device;
	ColorImage* m_drawImage;
	DepthImage* m_depthImage;
	Buffer* m_sceneDataUBO;
	Pipeline* m_pipeline;
	Swapchain* m_swapchain;

	struct FrameData {
		Semaphore* finishedRendering;
		Fence* waitForRenderingCompletion;
		Command* cmd;
	} frames[FRAMES_IN_FLIGHT];

	uint32_t m_currentFrame{0};

	FrameData& currFrame() { return frames[m_currentFrame % FRAMES_IN_FLIGHT]; }

	uint32_t graphicsQueue;
	uint32_t uploadQueue;

	DrawAttachment m_drawAttachment;
	DepthAttachment m_depthAttachment;

	PushConstants m_pushConstants;
};
