#include "scene.hpp"
#include "render_target.hpp"
#include "camera.hpp"

namespace etna {

struct RenderSettings {
	VkViewport viewport{};
	Color clearColor{Engine::ETNA_CLEAR_COLOR};
	VkAttachmentStoreOp colorStoreOp{VK_ATTACHMENT_STORE_OP_STORE};
	VkAttachmentStoreOp depthStoreOp{VK_ATTACHMENT_STORE_OP_DONT_CARE};
	VkAttachmentLoadOp colorLoadOp{VK_ATTACHMENT_LOAD_OP_CLEAR};
	VkAttachmentLoadOp depthLoadOp{VK_ATTACHMENT_LOAD_OP_CLEAR};
};

class Renderer {
public:
	Renderer();

	~Renderer();

	void beginFrame();

	void endFrame();

	void renderScene(const Scene&,
					 const RenderTarget&,
					 const Camera&,
					 const RenderSettings = {});

	ignis::Command& getCommand() const {
		return *m_framesData[m_currentFrame].m_cmd;
	}

private:
	struct FrameData {
		ignis::Fence* m_inFlight;
		ignis::Command* m_cmd;
	} m_framesData[Engine::ETNA_FRAMES_IN_FLIGHT];

	struct PushConstants {
		Mat4 worldTransform;
		ignis::BufferId vertices;
		ignis::BufferId material;
		ignis::BufferId sceneData;
	} m_pushConstants;

	uint32_t m_currentFrame{0};
};

}  // namespace etna
