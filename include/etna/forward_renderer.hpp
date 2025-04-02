#include "scene.hpp"
#include "render_target.hpp"
#include "engine.hpp"

namespace etna {

struct DirectionalLight {
	Vec3 direction;
	float padding;
	Color color;
};

struct RenderSettings {
	Color clearColor{engine::ETNA_CLEAR_COLOR};
	Color ambientColor;
	DirectionalLight sun;
	VkAttachmentStoreOp colorStoreOp{VK_ATTACHMENT_STORE_OP_STORE};
	VkAttachmentStoreOp depthStoreOp{VK_ATTACHMENT_STORE_OP_DONT_CARE};
	VkAttachmentLoadOp colorLoadOp{VK_ATTACHMENT_LOAD_OP_CLEAR};
	VkAttachmentLoadOp depthLoadOp{VK_ATTACHMENT_LOAD_OP_CLEAR};
	bool renderDepth{true};
};

constexpr RenderSettings LOAD_PREVIOUS{
	.colorLoadOp = VK_ATTACHMENT_LOAD_OP_LOAD,
	.depthLoadOp = VK_ATTACHMENT_LOAD_OP_LOAD,
};

class Renderer {
public:
	struct CreateInfo {
		uint32_t framesInFlight{2};
	};

	Renderer(const CreateInfo&);

	~Renderer();

	void beginFrame();

	void endFrame();

	void renderScene(const Scene&,
					 const RenderTarget&,
					 const CameraNode&,
					 const RenderSettings = {});

	ignis::Command& getCommand() const { return *m_framesData[m_currentFrame].cmd; }

private:
	uint32_t m_framesInFlight;
	uint32_t m_currentFrame{0};

	struct FrameData {
		ignis::Fence* inFlight;
		ignis::Command* cmd;
		ignis::BufferId sceneDataBuff;
	};

	std::vector<FrameData> m_framesData;

	struct PushConstants {
		Mat4 worldTransform;
		ignis::BufferId vertices;
		ignis::BufferId material;
		ignis::BufferId sceneData;
		ignis::BufferId cameraData;
	} m_pushConstants;

	std::vector<ignis::BufferId> m_cameraDataBuffs;
};

}  // namespace etna
