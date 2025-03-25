#pragma once

#include "mesh.hpp"
#include "material.hpp"
#include "ignis/image.hpp"
#include "ignis/device.hpp"

namespace etna {

struct Camera;
struct RenderTarget;
class Scene;
struct SceneData;

struct RenderSettings {
	VkViewport viewport{};
	Color clearColor{0.02f, 0.02f, 0.02f, 1};  // TODO: use ETNA_CLEAR_COLOR
	VkAttachmentStoreOp colorStoreOp{VK_ATTACHMENT_STORE_OP_STORE};
	VkAttachmentStoreOp depthStoreOp{VK_ATTACHMENT_STORE_OP_DONT_CARE};
	VkAttachmentLoadOp colorLoadOp{VK_ATTACHMENT_LOAD_OP_CLEAR};
	VkAttachmentLoadOp depthLoadOp{VK_ATTACHMENT_LOAD_OP_CLEAR};
};

class Engine {
public:
	Engine();

	~Engine();

	void beginFrame();

	void endFrame();

	void renderScene(const Scene&,
					 const RenderTarget&,
					 const Camera&,
					 const RenderSettings = {});

	Mesh createMesh(const _Mesh::CreateInfo&);

	Material createMaterial(const _Material::CreateInfo&);

	ignis::Command& getCommand() const {
		return *m_framesData[m_currentFrame].m_cmd;
	}

	static ignis::Device& getDevice();

	static VkQueue getGraphicsQueue();

	static VkQueue getUploadQueue();

	static constexpr uint32_t ETNA_FRAMES_IN_FLIGHT{2};

	static constexpr ignis::ColorFormat ETNA_COLOR_FORMAT{
		ignis::ColorFormat::RGBA16};

	static constexpr ignis::DepthFormat ETNA_DEPTH_FORMAT{
		ignis::DepthFormat::D32_SFLOAT};

	static constexpr Color ETNA_CLEAR_COLOR{0.02f, 0.02f, 0.02f, 1};

private:
	_Material* m_defaultMaterial{nullptr};

	struct FrameData {
		ignis::Fence* m_inFlight;
		ignis::Command* m_cmd;
	} m_framesData[ETNA_FRAMES_IN_FLIGHT];

	struct PushConstants {
		Mat4 worldTransform;
		ignis::BufferId vertices;
		ignis::BufferId material;
		ignis::BufferId sceneData;
	} m_pushConstants;

	uint32_t m_currentFrame{0};

public:
	Engine(const Engine&) = delete;
	Engine(Engine&&) = delete;
	Engine& operator=(const Engine&) = delete;
	Engine& operator=(Engine&&) = delete;
};

}  // namespace etna
