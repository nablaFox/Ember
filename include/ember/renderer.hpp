#pragma once

#include <cassert>
#include <chrono>
#include "material.hpp"
#include "types.hpp"
#include "mesh.hpp"
#include "window.hpp"
#include "camera.hpp"
#include "ignis/ignis.hpp"

namespace ember {

struct SceneData {
	Mat4 viewproj;
	Color ambientColor;
	DirectionalLight sun;
};

struct PushConstants {
	Mat4 worldTransform;
	VkDeviceAddress verticesAddress;
	uint32_t materialHandle;
};

class Renderer {
public:
	Renderer(const Window&);
	~Renderer();

public:
	// TODO: add BeginSceneInfo struct
	struct SceneInfo {
		DirectionalLight sun{};
		Color ambientColor{};
	};

	void beginScene(Camera camera, SceneInfo sceneInfo);

	void endScene();

	void draw(Mesh&, WorldTransform, const Material* = nullptr);

	float getFps() const { return m_fps; }

	auto getDeltaTime() const { return 1 / static_cast<float>(m_fps); };

	void updateFps();

private:
	ignis::ColorImage* m_drawImage;
	ignis::ColorImage* m_resolvedDrawImage;
	ignis::DepthImage* m_depthImage;

	ignis::Buffer* m_sceneDataUBO;
	ignis::Swapchain* m_swapchain;

	static constexpr uint32_t FRAMES_IN_FLIGHT = 2;

	struct FrameData {
		ignis::Semaphore* finishedRendering;
		ignis::Fence* waitForRenderingCompletion;
		ignis::Command* cmd;
	} frames[FRAMES_IN_FLIGHT];

	uint32_t m_currentFrame{0};

	auto& currFrame() const { return frames[m_currentFrame % FRAMES_IN_FLIGHT]; }

	ignis::DrawAttachment m_drawAttachment;
	ignis::DepthAttachment m_depthAttachment;

	PushConstants m_pushConstants;

	std::chrono::time_point<std::chrono::high_resolution_clock> m_lastTime =
		std::chrono::high_resolution_clock::now();

	uint32_t m_fps{0};
};

}  // namespace ember
