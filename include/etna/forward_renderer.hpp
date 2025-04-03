#include "ignis/fence.hpp"
#include "scene.hpp"
#include "render_target.hpp"
#include "color.hpp"

namespace etna {

struct DirectionalLight {
	Vec3 direction;
	float padding;
	Color color;
};

struct RenderFrameSettings {
	Color clearColor{0.02f, 0.02f, 0.02f, 1};
	Color ambientColor;
	DirectionalLight sun;
	VkAttachmentStoreOp colorStoreOp{VK_ATTACHMENT_STORE_OP_STORE};
	VkAttachmentStoreOp depthStoreOp{VK_ATTACHMENT_STORE_OP_DONT_CARE};
	VkAttachmentLoadOp colorLoadOp{VK_ATTACHMENT_LOAD_OP_CLEAR};
	VkAttachmentLoadOp depthLoadOp{VK_ATTACHMENT_LOAD_OP_CLEAR};
	bool renderDepth{true};
};

constexpr RenderFrameSettings LOAD_PREVIOUS{
	.colorLoadOp = VK_ATTACHMENT_LOAD_OP_LOAD,
};

struct RenderSettings {
	Viewport viewport;
	bool clearViewport{true};
	Color clearColor{0.02f, 0.02f, 0.02f, 1};
};

class Renderer {
public:
	struct CreateInfo {
		uint32_t framesInFlight{2};
	};

	Renderer(const CreateInfo&);

	~Renderer();

	void beginDrawing(const RenderTarget&,
					  const CameraNode&,
					  const RenderFrameSettings& = {});

	void endDrawing();

	void begin();

	void execute();

	void renderScene(const Scene&, const RenderSettings& = {});

	void renderMesh(const MeshNode&, const RenderSettings& = {});

	void renderInstanced(const MeshNode&,
						 ignis::BufferId instanceBuffer,
						 const Viewport& = {});

	void drawScene(const RenderTarget&,
				   const CameraNode&,
				   const Scene&,
				   const Viewport& = {},
				   const RenderFrameSettings& = {});

	ignis::Command& getCommand() const { return *m_framesData[m_currentFrame].cmd; }

private:
	uint32_t m_framesInFlight;
	uint32_t m_currentFrame{0};

	struct FrameData {
		ignis::Fence* inFlight;
		ignis::Command* cmd;
		ignis::BufferId sceneDataBuff;
	};

	const CameraNode* m_currCamera;
	const RenderTarget* m_currTarget;

	std::vector<FrameData> m_framesData;

	struct PushConstants {
		Mat4 worldTransform;
		ignis::BufferId vertices;
		ignis::BufferId material;
		ignis::BufferId sceneData;
		ignis::BufferId cameraData;
	} m_pushConstants;

	std::vector<ignis::BufferId> m_cameraDataBuffs;

public:
	Renderer(const Renderer&) = delete;
	Renderer& operator=(const Renderer&) = delete;
	Renderer(Renderer&&) = delete;
	Renderer& operator=(Renderer&&) = delete;
};

}  // namespace etna
